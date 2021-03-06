/*	$OpenBSD: man_validate.c,v 1.91 2015/10/12 00:07:27 schwarze Exp $ */
/*
 * Copyright (c) 2008, 2009, 2010, 2011 Kristaps Dzonsons <kristaps@bsd.lv>
 * Copyright (c) 2010, 2012-2015 Ingo Schwarze <schwarze@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHORS DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <sys/types.h>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mandoc_aux.h"
#include "mandoc.h"
#include "roff.h"
#include "man.h"
#include "libmandoc.h"
#include "roff_int.h"
#include "libman.h"

#define	CHKARGS	  struct roff_man *man, struct roff_node *n

typedef	void	(*v_check)(CHKARGS);

static	void	  check_par(CHKARGS);
static	void	  check_part(CHKARGS);
static	void	  check_root(CHKARGS);
static	void	  check_text(CHKARGS);

static	void	  post_AT(CHKARGS);
static	void	  post_IP(CHKARGS);
static	void	  post_vs(CHKARGS);
static	void	  post_ft(CHKARGS);
static	void	  post_OP(CHKARGS);
static	void	  post_TH(CHKARGS);
static	void	  post_UC(CHKARGS);
static	void	  post_UR(CHKARGS);

static	v_check man_valids[MAN_MAX] = {
	post_vs,    /* br */
	post_TH,    /* TH */
	NULL,       /* SH */
	NULL,       /* SS */
	NULL,       /* TP */
	check_par,  /* LP */
	check_par,  /* PP */
	check_par,  /* P */
	post_IP,    /* IP */
	NULL,       /* HP */
	NULL,       /* SM */
	NULL,       /* SB */
	NULL,       /* BI */
	NULL,       /* IB */
	NULL,       /* BR */
	NULL,       /* RB */
	NULL,       /* R */
	NULL,       /* B */
	NULL,       /* I */
	NULL,       /* IR */
	NULL,       /* RI */
	post_vs,    /* sp */
	NULL,       /* nf */
	NULL,       /* fi */
	NULL,       /* RE */
	check_part, /* RS */
	NULL,       /* DT */
	post_UC,    /* UC */
	NULL,       /* PD */
	post_AT,    /* AT */
	NULL,       /* in */
	post_ft,    /* ft */
	post_OP,    /* OP */
	NULL,       /* EX */
	NULL,       /* EE */
	post_UR,    /* UR */
	NULL,       /* UE */
	NULL,       /* ll */
};


void
man_node_validate(struct roff_man *man)
{
	struct roff_node *n;
	v_check		*cp;

	n = man->last;
	man->last = man->last->child;
	while (man->last != NULL) {
		man_node_validate(man);
		if (man->last == n)
			man->last = man->last->child;
		else
			man->last = man->last->next;
	}

	man->last = n;
	man->next = ROFF_NEXT_SIBLING;
	switch (n->type) {
	case ROFFT_TEXT:
		check_text(man, n);
		break;
	case ROFFT_ROOT:
		check_root(man, n);
		break;
	case ROFFT_EQN:
	case ROFFT_TBL:
		break;
	default:
		cp = man_valids + n->tok;
		if (*cp)
			(*cp)(man, n);
		if (man->last == n)
			man_state(man, n);
		break;
	}
}

static void
check_root(CHKARGS)
{

	assert((man->flags & (MAN_BLINE | MAN_ELINE)) == 0);

	if (NULL == man->first->child)
		mandoc_msg(MANDOCERR_DOC_EMPTY, man->parse,
		    n->line, n->pos, NULL);
	else
		man->meta.hasbody = 1;

	if (NULL == man->meta.title) {
		mandoc_msg(MANDOCERR_TH_NOTITLE, man->parse,
		    n->line, n->pos, NULL);

		/*
		 * If a title hasn't been set, do so now (by
		 * implication, date and section also aren't set).
		 */

		man->meta.title = mandoc_strdup("");
		man->meta.msec = mandoc_strdup("");
		man->meta.date = man->quick ? mandoc_strdup("") :
		    mandoc_normdate(man->parse, NULL, n->line, n->pos);
	}
}

static void
check_text(CHKARGS)
{
	char		*cp, *p;

	if (MAN_LITERAL & man->flags)
		return;

	cp = n->string;
	for (p = cp; NULL != (p = strchr(p, '\t')); p++)
		mandoc_msg(MANDOCERR_FI_TAB, man->parse,
		    n->line, n->pos + (p - cp), NULL);
}

static void
post_OP(CHKARGS)
{

	if (n->nchild == 0)
		mandoc_msg(MANDOCERR_OP_EMPTY, man->parse,
		    n->line, n->pos, "OP");
	else if (n->nchild > 2) {
		n = n->child->next->next;
		mandoc_vmsg(MANDOCERR_ARG_EXCESS, man->parse,
		    n->line, n->pos, "OP ... %s", n->string);
	}
}

static void
post_UR(CHKARGS)
{

	if (n->type == ROFFT_HEAD && n->child == NULL)
		mandoc_vmsg(MANDOCERR_UR_NOHEAD, man->parse,
		    n->line, n->pos, "UR");
	check_part(man, n);
}

static void
post_ft(CHKARGS)
{
	char	*cp;
	int	 ok;

	if (0 == n->nchild)
		return;

	ok = 0;
	cp = n->child->string;
	switch (*cp) {
	case '1':
	case '2':
	case '3':
	case '4':
	case 'I':
	case 'P':
	case 'R':
		if ('\0' == cp[1])
			ok = 1;
		break;
	case 'B':
		if ('\0' == cp[1] || ('I' == cp[1] && '\0' == cp[2]))
			ok = 1;
		break;
	case 'C':
		if ('W' == cp[1] && '\0' == cp[2])
			ok = 1;
		break;
	default:
		break;
	}

	if (0 == ok) {
		mandoc_vmsg(MANDOCERR_FT_BAD, man->parse,
		    n->line, n->pos, "ft %s", cp);
		*cp = '\0';
	}
}

static void
check_part(CHKARGS)
{

	if (n->type == ROFFT_BODY && n->child == NULL)
		mandoc_msg(MANDOCERR_BLK_EMPTY, man->parse,
		    n->line, n->pos, man_macronames[n->tok]);
}

static void
check_par(CHKARGS)
{

	switch (n->type) {
	case ROFFT_BLOCK:
		if (0 == n->body->nchild)
			roff_node_delete(man, n);
		break;
	case ROFFT_BODY:
		if (0 == n->nchild)
			mandoc_vmsg(MANDOCERR_PAR_SKIP,
			    man->parse, n->line, n->pos,
			    "%s empty", man_macronames[n->tok]);
		break;
	case ROFFT_HEAD:
		if (n->nchild)
			mandoc_vmsg(MANDOCERR_ARG_SKIP,
			    man->parse, n->line, n->pos,
			    "%s %s%s", man_macronames[n->tok],
			    n->child->string,
			    n->nchild > 1 ? " ..." : "");
		break;
	default:
		break;
	}
}

static void
post_IP(CHKARGS)
{

	switch (n->type) {
	case ROFFT_BLOCK:
		if (0 == n->head->nchild && 0 == n->body->nchild)
			roff_node_delete(man, n);
		break;
	case ROFFT_BODY:
		if (0 == n->parent->head->nchild && 0 == n->nchild)
			mandoc_vmsg(MANDOCERR_PAR_SKIP,
			    man->parse, n->line, n->pos,
			    "%s empty", man_macronames[n->tok]);
		break;
	default:
		break;
	}
}

static void
post_TH(CHKARGS)
{
	struct roff_node *nb;
	const char	*p;

	free(man->meta.title);
	free(man->meta.vol);
	free(man->meta.os);
	free(man->meta.msec);
	free(man->meta.date);

	man->meta.title = man->meta.vol = man->meta.date =
	    man->meta.msec = man->meta.os = NULL;

	nb = n;

	/* ->TITLE<- MSEC DATE OS VOL */

	n = n->child;
	if (n && n->string) {
		for (p = n->string; '\0' != *p; p++) {
			/* Only warn about this once... */
			if (isalpha((unsigned char)*p) &&
			    ! isupper((unsigned char)*p)) {
				mandoc_vmsg(MANDOCERR_TITLE_CASE,
				    man->parse, n->line,
				    n->pos + (p - n->string),
				    "TH %s", n->string);
				break;
			}
		}
		man->meta.title = mandoc_strdup(n->string);
	} else {
		man->meta.title = mandoc_strdup("");
		mandoc_msg(MANDOCERR_TH_NOTITLE, man->parse,
		    nb->line, nb->pos, "TH");
	}

	/* TITLE ->MSEC<- DATE OS VOL */

	if (n)
		n = n->next;
	if (n && n->string)
		man->meta.msec = mandoc_strdup(n->string);
	else {
		man->meta.msec = mandoc_strdup("");
		mandoc_vmsg(MANDOCERR_MSEC_MISSING, man->parse,
		    nb->line, nb->pos, "TH %s", man->meta.title);
	}

	/* TITLE MSEC ->DATE<- OS VOL */

	if (n)
		n = n->next;
	if (n && n->string && '\0' != n->string[0]) {
		man->meta.date = man->quick ?
		    mandoc_strdup(n->string) :
		    mandoc_normdate(man->parse, n->string,
			n->line, n->pos);
	} else {
		man->meta.date = mandoc_strdup("");
		mandoc_msg(MANDOCERR_DATE_MISSING, man->parse,
		    n ? n->line : nb->line,
		    n ? n->pos : nb->pos, "TH");
	}

	/* TITLE MSEC DATE ->OS<- VOL */

	if (n && (n = n->next))
		man->meta.os = mandoc_strdup(n->string);
	else if (man->defos != NULL)
		man->meta.os = mandoc_strdup(man->defos);

	/* TITLE MSEC DATE OS ->VOL<- */
	/* If missing, use the default VOL name for MSEC. */

	if (n && (n = n->next))
		man->meta.vol = mandoc_strdup(n->string);
	else if ('\0' != man->meta.msec[0] &&
	    (NULL != (p = mandoc_a2msec(man->meta.msec))))
		man->meta.vol = mandoc_strdup(p);

	if (n != NULL && (n = n->next) != NULL)
		mandoc_vmsg(MANDOCERR_ARG_EXCESS, man->parse,
		    n->line, n->pos, "TH ... %s", n->string);

	/*
	 * Remove the `TH' node after we've processed it for our
	 * meta-data.
	 */
	roff_node_delete(man, man->last);
}

static void
post_UC(CHKARGS)
{
	static const char * const bsd_versions[] = {
	    "3rd Berkeley Distribution",
	    "4th Berkeley Distribution",
	    "4.2 Berkeley Distribution",
	    "4.3 Berkeley Distribution",
	    "4.4 Berkeley Distribution",
	};

	const char	*p, *s;

	n = n->child;

	if (n == NULL || n->type != ROFFT_TEXT)
		p = bsd_versions[0];
	else {
		s = n->string;
		if (0 == strcmp(s, "3"))
			p = bsd_versions[0];
		else if (0 == strcmp(s, "4"))
			p = bsd_versions[1];
		else if (0 == strcmp(s, "5"))
			p = bsd_versions[2];
		else if (0 == strcmp(s, "6"))
			p = bsd_versions[3];
		else if (0 == strcmp(s, "7"))
			p = bsd_versions[4];
		else
			p = bsd_versions[0];
	}

	free(man->meta.os);
	man->meta.os = mandoc_strdup(p);
}

static void
post_AT(CHKARGS)
{
	static const char * const unix_versions[] = {
	    "7th Edition",
	    "System III",
	    "System V",
	    "System V Release 2",
	};

	struct roff_node *nn;
	const char	*p, *s;

	n = n->child;

	if (n == NULL || n->type != ROFFT_TEXT)
		p = unix_versions[0];
	else {
		s = n->string;
		if (0 == strcmp(s, "3"))
			p = unix_versions[0];
		else if (0 == strcmp(s, "4"))
			p = unix_versions[1];
		else if (0 == strcmp(s, "5")) {
			nn = n->next;
			if (nn != NULL &&
			    nn->type == ROFFT_TEXT &&
			    nn->string[0] != '\0')
				p = unix_versions[3];
			else
				p = unix_versions[2];
		} else
			p = unix_versions[0];
	}

	free(man->meta.os);
	man->meta.os = mandoc_strdup(p);
}

static void
post_vs(CHKARGS)
{

	if (NULL != n->prev)
		return;

	switch (n->parent->tok) {
	case MAN_SH:
	case MAN_SS:
		mandoc_vmsg(MANDOCERR_PAR_SKIP, man->parse, n->line, n->pos,
		    "%s after %s", man_macronames[n->tok],
		    man_macronames[n->parent->tok]);
		/* FALLTHROUGH */
	case TOKEN_NONE:
		/*
		 * Don't warn about this because it occurs in pod2man
		 * and would cause considerable (unfixable) warnage.
		 */
		roff_node_delete(man, n);
		break;
	default:
		break;
	}
}
