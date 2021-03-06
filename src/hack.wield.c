/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* hack.wield.c - version 1.0.3 */
/* $FreeBSD: src/games/hack/hack.wield.c,v 1.4 1999/11/16 10:26:38 marcel Exp $ */
/* $DragonFly: src/games/hack/hack.wield.c,v 1.4 2006/08/21 19:45:32 pavalos Exp $ */

#include	"hack.h"
extern struct obj zeroobj;

void
setuwep(struct obj *obj)
{
	setworn(obj, W_WEP);
}

int
dowield(void)
{
	struct obj *wep;
	int res = 0;

	multi = 0;
	if(!(wep = getobj("#-)", "wield"))) /* nothing */;
	else if(uwep == wep)
		pline("You are already wielding that!");
	else if(wep == &zeroobj) {
		if(uwep == 0){
			pline("You are already empty handed.");
		} else {
			setuwep(NULL);
			res++;
			pline("You are empty handed.");
		}
	} else if(uarms && wep->otyp == TWO_HANDED_SWORD)
	pline("You cannot wield a two-handed sword and wear a shield.");
	else if(wep->owornmask & (W_ARMOR | W_RING))
		pline("You cannot wield that!");
	else {
		setuwep(wep);
		res++;
		prinv(uwep);
	}
	return(res);
}

void
corrode_weapon(void)
{
	if(!uwep || uwep->olet != WEAPON_SYM) return;	/* %% */
	if(uwep->rustfree)
		pline("Your %s not affected.", aobjnam(uwep, "are"));
	else {
		pline("Your %s!", aobjnam(uwep, "corrode"));
		uwep->spe--;
	}
}

bool
chwepon(struct obj *otmp, int amount)
{
	const char *color = (amount < 0) ? "black" : "green";
	const char *ltime;

	if(!uwep || uwep->olet != WEAPON_SYM) {
		strange_feeling(otmp,
			(amount > 0) ? "Your hands twitch."
				     : "Your hands itch.");
		return(0);
	}

	if(uwep->otyp == WORM_TOOTH && amount > 0) {
		uwep->otyp = CRYSKNIFE;
		pline("Your weapon seems sharper now.");
		return(1);
	}

	if(uwep->otyp == CRYSKNIFE && amount < 0) {
		uwep->otyp = WORM_TOOTH;
		pline("Your weapon looks duller now.");
		return(1);
	}

	/* there is a (soft) upper limit to uwep->spe */
	if(amount > 0 && uwep->spe > 5 && rn2(3)) {
	    pline("Your %s violently green for a while and then evaporate%s.",
		aobjnam(uwep, "glow"), plur(uwep->quan));
	    while(uwep)		/* let all of them disappear */
				/* note: uwep->quan = 1 is nogood if unpaid */
	        useup(uwep);
	    return(1);
	}
	if(!rn2(6)) amount *= 2;
	ltime = (amount*amount == 1) ? "moment" : "while";
	pline("Your %s %s for a %s.",
		aobjnam(uwep, "glow"), color, ltime);
	uwep->spe += amount;
	return(1);
}
