/*	$OpenBSD: table.c,v 1.20 2015/11/23 21:50:12 gilles Exp $	*/

/*
 * Copyright (c) 2013 Eric Faurot <eric@openbsd.org>
 * Copyright (c) 2008 Gilles Chehade <gilles@poolp.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/tree.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <event.h>
#include <imsg.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#include "smtpd.h"
#include "log.h"

struct table_backend *table_backend_lookup(const char *);

extern struct table_backend table_backend_static;
extern struct table_backend table_backend_db;
extern struct table_backend table_backend_getpwnam;
extern struct table_backend table_backend_proc;

static const char * table_service_name(enum table_service);
static const char * table_backend_name(struct table_backend *);
static const char * table_dump_lookup(enum table_service, union lookup *);
static int parse_sockaddr(struct sockaddr *, int, const char *);

static unsigned int last_table_id = 0;

struct table_backend *
table_backend_lookup(const char *backend)
{
	if (!strcmp(backend, "static") || !strcmp(backend, "file"))
		return &table_backend_static;
	if (!strcmp(backend, "db"))
		return &table_backend_db;
	if (!strcmp(backend, "getpwnam"))
		return &table_backend_getpwnam;
	if (!strcmp(backend, "proc"))
		return &table_backend_proc;
	return NULL;
}

static const char *
table_backend_name(struct table_backend *backend)
{
	if (backend == &table_backend_static)
		return "static";
	if (backend == &table_backend_db)
		return "db";
	if (backend == &table_backend_getpwnam)
		return "getpwnam";
	if (backend == &table_backend_proc)
		return "proc";
	return "???";
}

static const char *
table_service_name(enum table_service s)
{
	switch (s) {
	case K_NONE:		return "NONE";
	case K_ALIAS:		return "ALIAS";
	case K_DOMAIN:		return "DOMAIN";
	case K_CREDENTIALS:	return "CREDENTIALS";
	case K_NETADDR:		return "NETADDR";
	case K_USERINFO:	return "USERINFO";
	case K_SOURCE:		return "SOURCE";
	case K_MAILADDR:	return "MAILADDR";
	case K_ADDRNAME:	return "ADDRNAME";
	case K_MAILADDRMAP:	return "MAILADDRMAP";
	default:		return "???";
	}
}

struct table *
table_find(const char *name, const char *tag)
{
	char buf[LINE_MAX];

	if (tag == NULL)
		return dict_get(env->sc_tables_dict, name);

	if ((size_t)snprintf(buf, sizeof(buf), "%s#%s", name, tag) >= sizeof(buf)) {
		log_warnx("warn: table name too long: %s#%s", name, tag);
		return (NULL);
	}

	return dict_get(env->sc_tables_dict, buf);
}

int
table_lookup(struct table *table, struct dict *params, const char *key, enum table_service kind,
    union lookup *lk)
{
	int	r;
	char	lkey[1024];

	if (table->t_backend->lookup == NULL)
		return (-1);

	if (! lowercase(lkey, key, sizeof lkey)) {
		log_warnx("warn: lookup key too long: %s", key);
		return -1;
	}

	r = table->t_backend->lookup(table->t_handle, params, lkey, kind, lk);

	if (r == 1)
		log_trace(TRACE_LOOKUP, "lookup: %s \"%s\" as %s in table %s:%s -> %s%s%s",
		    lk ? "lookup" : "check",
		    lkey,
		    table_service_name(kind),
		    table_backend_name(table->t_backend),
		    table->t_name,
		    lk ? "\"" : "",
		    (lk) ? table_dump_lookup(kind, lk): "found",
		    lk ? "\"" : "");
	else
		log_trace(TRACE_LOOKUP, "lookup: %s \"%s\" as %s in table %s:%s -> %d",
		    lk ? "lookup" : "check",
		    lkey,
		    table_service_name(kind),
		    table_backend_name(table->t_backend),
		    table->t_name,
		    r);

	return (r);
}

int
table_fetch(struct table *table, struct dict *params, enum table_service kind, union lookup *lk)
{
	int 	r;

	if (table->t_backend->fetch == NULL)
		return (-1);

	r = table->t_backend->fetch(table->t_handle, params, kind, lk);

	if (r == 1)
		log_trace(TRACE_LOOKUP, "lookup: fetch %s from table %s:%s -> %s%s%s",
		    table_service_name(kind),
		    table_backend_name(table->t_backend),
		    table->t_name,
		    lk ? "\"" : "",
		    (lk) ? table_dump_lookup(kind, lk): "found",
		    lk ? "\"" : "");
	else
		log_trace(TRACE_LOOKUP, "lookup: fetch %s from table %s:%s -> %d",
		    table_service_name(kind),
		    table_backend_name(table->t_backend),
		    table->t_name,
		    r);

	return (r);
}

struct table *
table_create(const char *backend, const char *name, const char *tag,
    const char *config)
{
	struct table		*t;
	struct table_backend	*tb;
	char			*ps;
	char			*p;
	char			 buf[LINE_MAX];
	char			 path[LINE_MAX];
	char			 pathsplit[LINE_MAX];
	size_t			 n;
	struct stat		 sb;

	if (name && tag) {
		if ((size_t)snprintf(buf, sizeof(buf), "%s#%s", name, tag) >=
		    sizeof(buf))
			fatalx("table_create: name too long \"%s#%s\"",
			    name, tag);
		name = buf;
	}

	if (name && table_find(name, NULL))
		fatalx("table_create: table \"%s\" already defined", name);

	if ((tb = table_backend_lookup(backend)) == NULL) {
		(void)strlcpy(pathsplit, PATH_LIBEXEC, sizeof pathsplit);
		for (ps = pathsplit; (p = strsep(&ps, ":")) != NULL;)  {
			if ((size_t)snprintf(path, sizeof(path), "%s/table-%s",
				p, backend) >= sizeof(path)) {
				fatalx("table_create: path too long \""
				    "%s/table-%s\"", p, backend);
			}
			if (stat(path, &sb) == 0) {
				tb = table_backend_lookup("proc");
				(void)strlcpy(path, backend, sizeof(path));
				if (config) {
					(void)strlcat(path, ":", sizeof(path));
					if (strlcat(path, config, sizeof(path))
					    >= sizeof(path))
						fatalx("table_create: config file path too long");
				}
				config = path;
				break;
			}
		}
	}

	if (tb == NULL)
		fatalx("table_create: backend \"%s\" does not exist", backend);

	t = xcalloc(1, sizeof(*t), "table_create");
	t->t_backend = tb;

	/* XXX */
	/*
	 * until people forget about it, "file" really means "static"
	 */
	if (!strcmp(backend, "file"))
		backend = "static";

	if (config) {
		if (strlcpy(t->t_config, config, sizeof t->t_config)
		    >= sizeof t->t_config)
			fatalx("table_create: table config \"%s\" too large",
			    t->t_config);
	}

	if (strcmp(backend, "static") != 0)
		t->t_type = T_DYNAMIC;

	if (name == NULL)
		(void)snprintf(t->t_name, sizeof(t->t_name), "<dynamic:%u>",
		    last_table_id++);
	else {
		n = strlcpy(t->t_name, name, sizeof(t->t_name));
		if (n >= sizeof(t->t_name))
			fatalx("table_create: table name too long");
	}

	dict_init(&t->t_dict);
	dict_set(env->sc_tables_dict, t->t_name, t);

	return (t);
}

void
table_destroy(struct table *t)
{
	void	*p = NULL;

	while (dict_poproot(&t->t_dict, (void **)&p))
		free(p);

	dict_xpop(env->sc_tables_dict, t->t_name);
	free(t);
}

int
table_config(struct table *t)
{
	if (t->t_backend->config == NULL)
		return (1);
	return (t->t_backend->config(t));
}

void
table_add(struct table *t, const char *key, const char *val)
{
	char	lkey[1024], *old;

	if (t->t_type & T_DYNAMIC)
		fatalx("table_add: cannot add to table");

	if (! lowercase(lkey, key, sizeof lkey)) {
		log_warnx("warn: lookup key too long: %s", key);
		return;
	}

	old = dict_set(&t->t_dict, lkey, val ? xstrdup(val, "table_add") : NULL);
	if (old) {
		log_warnx("warn: duplicate key \"%s\" in static table \"%s\"",
		    lkey, t->t_name);
		free(old);
	}
}

int
table_check_type(struct table *t, uint32_t mask)
{
	return t->t_type & mask;
}

int
table_check_service(struct table *t, uint32_t mask)
{
	return t->t_backend->services & mask;
}

int
table_check_use(struct table *t, uint32_t tmask, uint32_t smask)
{
	return table_check_type(t, tmask) && table_check_service(t, smask);
}

int
table_open(struct table *t)
{
	t->t_handle = NULL;
	if (t->t_backend->open == NULL)
		return (1);
	t->t_handle = t->t_backend->open(t);
	if (t->t_handle == NULL)
		return (0);
	return (1);
}

void
table_close(struct table *t)
{
	if (t->t_backend->close)
		t->t_backend->close(t->t_handle);
}

int
table_update(struct table *t)
{
	if (t->t_backend->update == NULL)
		return (1);
	return (t->t_backend->update(t));
}


/*
 * quick reminder:
 * in *_match() s1 comes from session, s2 comes from table
 */

int
table_domain_match(const char *s1, const char *s2)
{
	return hostname_match(s1, s2);
}

int
table_mailaddr_match(const char *s1, const char *s2)
{
	struct mailaddr m1;
	struct mailaddr m2;

	if (! text_to_mailaddr(&m1, s1))
		return 0;
	if (! text_to_mailaddr(&m2, s2))
		return 0;
	return mailaddr_match(&m1, &m2);
}

static int table_match_mask(struct sockaddr_storage *, struct netaddr *);
static int table_inet4_match(struct sockaddr_in *, struct netaddr *);
static int table_inet6_match(struct sockaddr_in6 *, struct netaddr *);

int
table_netaddr_match(const char *s1, const char *s2)
{
	struct netaddr n1;
	struct netaddr n2;

	if (strcasecmp(s1, s2) == 0)
		return 1;
	if (! text_to_netaddr(&n1, s1))
		return 0;
	if (! text_to_netaddr(&n2, s2))
		return 0;
	if (n1.ss.ss_family != n2.ss.ss_family)
		return 0;
	if (n1.ss.ss_len != n2.ss.ss_len)
		return 0;
	return table_match_mask(&n1.ss, &n2);
}

static int
table_match_mask(struct sockaddr_storage *ss, struct netaddr *ssmask)
{
	if (ss->ss_family == AF_INET)
		return table_inet4_match((struct sockaddr_in *)ss, ssmask);

	if (ss->ss_family == AF_INET6)
		return table_inet6_match((struct sockaddr_in6 *)ss, ssmask);

	return (0);
}

static int
table_inet4_match(struct sockaddr_in *ss, struct netaddr *ssmask)
{
	in_addr_t mask;
	int i;

	/* a.b.c.d/8 -> htonl(0xff000000) */
	mask = 0;
	for (i = 0; i < ssmask->bits; ++i)
		mask = (mask >> 1) | 0x80000000;
	mask = htonl(mask);

	/* (addr & mask) == (net & mask) */
	if ((ss->sin_addr.s_addr & mask) ==
	    (((struct sockaddr_in *)ssmask)->sin_addr.s_addr & mask))
		return 1;

	return 0;
}

static int
table_inet6_match(struct sockaddr_in6 *ss, struct netaddr *ssmask)
{
	struct in6_addr	*in;
	struct in6_addr	*inmask;
	struct in6_addr	 mask;
	int		 i;

	memset(&mask, 0, sizeof(mask));
	for (i = 0; i < ssmask->bits / 8; i++)
		mask.s6_addr[i] = 0xff;
	i = ssmask->bits % 8;
	if (i)
		mask.s6_addr[ssmask->bits / 8] = 0xff00 >> i;

	in = &ss->sin6_addr;
	inmask = &((struct sockaddr_in6 *)&ssmask->ss)->sin6_addr;

	for (i = 0; i < 16; i++) {
		if ((in->s6_addr[i] & mask.s6_addr[i]) !=
		    (inmask->s6_addr[i] & mask.s6_addr[i]))
			return (0);
	}

	return (1);
}

void
table_dump_all(void)
{
	struct table	*t;
	void		*iter, *i2;
	const char 	*key, *sep;
	char		*value;
	char		 buf[1024];

	iter = NULL;
	while (dict_iter(env->sc_tables_dict, &iter, NULL, (void **)&t)) {
		i2 = NULL;
		sep = "";
 		buf[0] = '\0';
		if (t->t_type & T_DYNAMIC) {
			(void)strlcat(buf, "DYNAMIC", sizeof(buf));
			sep = ",";
		}
		if (t->t_type & T_LIST) {
			(void)strlcat(buf, sep, sizeof(buf));
			(void)strlcat(buf, "LIST", sizeof(buf));
			sep = ",";
		}
		if (t->t_type & T_HASH) {
			(void)strlcat(buf, sep, sizeof(buf));
			(void)strlcat(buf, "HASH", sizeof(buf));
			sep = ",";
		}
		log_debug("TABLE \"%s\" type=%s config=\"%s\"",
		    t->t_name, buf, t->t_config);
		while(dict_iter(&t->t_dict, &i2, &key, (void**)&value)) {
			if (value)
				log_debug("	\"%s\" -> \"%s\"", key, value);
			else
				log_debug("	\"%s\"", key);
		}
	}
}

void
table_open_all(void)
{
	struct table	*t;
	void		*iter;

	iter = NULL;
	while (dict_iter(env->sc_tables_dict, &iter, NULL, (void **)&t))
		if (! table_open(t))
			fatalx("failed to open table %s", t->t_name);
}

void
table_close_all(void)
{
	struct table	*t;
	void		*iter;

	iter = NULL;
	while (dict_iter(env->sc_tables_dict, &iter, NULL, (void **)&t))
		table_close(t);
}

int
table_parse_lookup(enum table_service service, const char *key,
    const char *line, union lookup *lk)
{
	char	buffer[LINE_MAX], *p;
	size_t	len;

	len = strlen(line);

	switch (service) {
	case K_ALIAS:
		lk->expand = calloc(1, sizeof(*lk->expand));
		if (lk->expand == NULL)
			return (-1);
		if (!expand_line(lk->expand, line, 1)) {
			expand_free(lk->expand);
			return (-1);
		}
		return (1);

	case K_DOMAIN:
		if (strlcpy(lk->domain.name, line, sizeof(lk->domain.name))
		    >= sizeof(lk->domain.name))
			return (-1);
		return (1);

	case K_CREDENTIALS:

		/* credentials are stored as user:password */
		if (len < 3)
			return (-1);

		/* too big to fit in a smtp session line */
		if (len >= LINE_MAX)
			return (-1);

		p = strchr(line, ':');
		if (p == NULL) {
			if (strlcpy(lk->creds.username, key, sizeof (lk->creds.username))
			    >= sizeof (lk->creds.username))
				return (-1);
			if (strlcpy(lk->creds.password, line, sizeof(lk->creds.password))
			    >= sizeof(lk->creds.password))
				return (-1);
			return (1);
		}

		if (p == line || p == line + len - 1)
			return (-1);

		memmove(lk->creds.username, line, p - line);
		lk->creds.username[p - line] = '\0';

		if (strlcpy(lk->creds.password, p+1, sizeof(lk->creds.password))
		    >= sizeof(lk->creds.password))
			return (-1);

		return (1);

	case K_NETADDR:
		if (!text_to_netaddr(&lk->netaddr, line))
			return (-1);
		return (1);

	case K_USERINFO:
		if (!bsnprintf(buffer, sizeof(buffer), "%s:%s", key, line))
			return (-1);
		if (!text_to_userinfo(&lk->userinfo, buffer))
			return (-1);
 		return (1);

	case K_SOURCE:
		if (parse_sockaddr((struct sockaddr *)&lk->source.addr,
		    PF_UNSPEC, line) == -1)
			return (-1);
		return (1);

	case K_MAILADDR:
		if (!text_to_mailaddr(&lk->mailaddr, line))
			return (-1);
		return (1);

	case K_MAILADDRMAP:
		lk->maddrmap = calloc(1, sizeof(*lk->maddrmap));
		if (lk->maddrmap == NULL)
			return (-1);
		maddrmap_init(lk->maddrmap);
		if (! mailaddr_line(lk->maddrmap, line)) {
			maddrmap_free(lk->maddrmap);
			return (-1);
		}
		return (1);

	case K_ADDRNAME:
		if (parse_sockaddr((struct sockaddr *)&lk->addrname.addr,
		    PF_UNSPEC, key) == -1)
			return (-1);
		if (strlcpy(lk->addrname.name, line, sizeof(lk->addrname.name))
		    >= sizeof(lk->addrname.name))
			return (-1);
		return (1);

	default:
		return (-1);
	}
}

static const char *
table_dump_lookup(enum table_service s, union lookup *lk)
{
	static char	buf[LINE_MAX];
	int		ret;

	switch (s) {
	case K_NONE:
		break;

	case K_ALIAS:
		expand_to_text(lk->expand, buf, sizeof(buf));
		break;

	case K_DOMAIN:
		ret = snprintf(buf, sizeof(buf), "%s", lk->domain.name);
		if (ret == -1 || (size_t)ret >= sizeof (buf))
			goto err;
		break;

	case K_CREDENTIALS:
		ret = snprintf(buf, sizeof(buf), "%s:%s",
		    lk->creds.username, lk->creds.password);
		if (ret == -1 || (size_t)ret >= sizeof (buf))
			goto err;
		break;

	case K_NETADDR:
		ret = snprintf(buf, sizeof(buf), "%s/%d",
		    sockaddr_to_text((struct sockaddr *)&lk->netaddr.ss),
		    lk->netaddr.bits);
		if (ret == -1 || (size_t)ret >= sizeof (buf))
			goto err;
		break;

	case K_USERINFO:
		ret = snprintf(buf, sizeof(buf), "%s:%d:%d:%s",
		    lk->userinfo.username,
		    lk->userinfo.uid,
		    lk->userinfo.gid,
		    lk->userinfo.directory);
		if (ret == -1 || (size_t)ret >= sizeof (buf))
			goto err;
		break;

	case K_SOURCE:
		ret = snprintf(buf, sizeof(buf), "%s",
		    ss_to_text(&lk->source.addr));
		if (ret == -1 || (size_t)ret >= sizeof (buf))
			goto err;
		break;

	case K_MAILADDR:
		ret = snprintf(buf, sizeof(buf), "%s@%s",
		    lk->mailaddr.user,
		    lk->mailaddr.domain);
		if (ret == -1 || (size_t)ret >= sizeof (buf))
			goto err;
		break;

	case K_ADDRNAME:
		ret = snprintf(buf, sizeof(buf), "%s",
		    lk->addrname.name);
		if (ret == -1 || (size_t)ret >= sizeof (buf))
			goto err;
		break;

	default:
		break;
	}

	return (buf);

err:
	return (NULL);
}


static int
parse_sockaddr(struct sockaddr *sa, int family, const char *str)
{
	struct in_addr		 ina;
	struct in6_addr		 in6a;
	struct sockaddr_in	*sin;
	struct sockaddr_in6	*sin6;
	char			*cp, *str2;
	const char		*errstr;

	switch (family) {
	case PF_UNSPEC:
		if (parse_sockaddr(sa, PF_INET, str) == 0)
			return (0);
		return parse_sockaddr(sa, PF_INET6, str);

	case PF_INET:
		if (inet_pton(PF_INET, str, &ina) != 1)
			return (-1);

		sin = (struct sockaddr_in *)sa;
		memset(sin, 0, sizeof *sin);
		sin->sin_len = sizeof(struct sockaddr_in);
		sin->sin_family = PF_INET;
		sin->sin_addr.s_addr = ina.s_addr;
		return (0);

	case PF_INET6:
		if (strncasecmp("ipv6:", str, 5) == 0)
			str += 5;
		cp = strchr(str, SCOPE_DELIMITER);
		if (cp) {
			str2 = strdup(str);
			if (str2 == NULL)
				return (-1);
			str2[cp - str] = '\0';
			if (inet_pton(PF_INET6, str2, &in6a) != 1) {
				free(str2);
				return (-1);
			}
			cp++;
			free(str2);
		} else if (inet_pton(PF_INET6, str, &in6a) != 1)
			return (-1);

		sin6 = (struct sockaddr_in6 *)sa;
		memset(sin6, 0, sizeof *sin6);
		sin6->sin6_len = sizeof(struct sockaddr_in6);
		sin6->sin6_family = PF_INET6;
		sin6->sin6_addr = in6a;

		if (cp == NULL)
			return (0);

		if (IN6_IS_ADDR_LINKLOCAL(&in6a) ||
		    IN6_IS_ADDR_MC_LINKLOCAL(&in6a) ||
		    IN6_IS_ADDR_MC_INTFACELOCAL(&in6a))
			if ((sin6->sin6_scope_id = if_nametoindex(cp)))
				return (0);

		sin6->sin6_scope_id = strtonum(cp, 0, UINT32_MAX, &errstr);
		if (errstr)
			return (-1);
		return (0);

	default:
		break;
	}

	return (-1);
}
