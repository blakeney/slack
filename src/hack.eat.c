/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* hack.eat.c - version 1.0.3 */
/* $FreeBSD: src/games/hack/hack.eat.c,v 1.4 1999/11/16 10:26:36 marcel Exp $ */
/* $DragonFly: src/games/hack/hack.eat.c,v 1.5 2006/08/21 19:45:32 pavalos Exp $ */

#include	"hack.h"
char POISONOUS[] = "ADKSVabhks";

static bool	opentin(void);
static void	Meatdone(void);
static int	eatcorpse(struct obj *);

#define	TTSZ	SIZE(tintxts)
struct { const char *txt; int nut; } tintxts[] = {
	{ "It contains first quality peaches - what a surprise!", 40 },
	{ "It contains salmon - not bad!", 60 },
	{ "It contains apple juice - perhaps not what you hoped for.", 20 },
	{ "It contains some nondescript substance, tasting awfully.", 500 },
	{ "It contains rotten meat. You vomit.", -50 },
	{ "It turns out to be empty.", 0 }
};

static struct {
	struct obj *tin;
	int usedtime, reqtime;
} tin;

static bool
opentin(void)
{
	int r;

	if(!carried(tin.tin))		/* perhaps it was stolen? */
		return(0);		/* %% probably we should use tinoid */
	if(tin.usedtime++ >= 50) {
		pline("You give up your attempt to open the tin.");
		return(0);
	}
	if(tin.usedtime < tin.reqtime)
		return(1);		/* still busy */

	pline("You succeed in opening the tin.");
	useup(tin.tin);
	r = rn2(2*TTSZ);
	if(r < TTSZ){
	    pline(tintxts[r].txt);
	    if(r == 1)	/* SALMON */ {
		Glib = rnd(15);
		pline("Eating salmon made your fingers very slippery.");
	    }
	} else {
	    pline("It contains spinach - this makes you feel like Popeye!");
	    if(u.ustr < 118)
		u.ustr += rnd( ((u.ustr < 17) ? 19 : 118) - u.ustr);
	    if(u.ustr > u.ustrmax) u.ustrmax = u.ustr;
	    flags.botl = 1;
	}
	return(0);
}

static void
Meatdone(void)
{
	u.usym = '@';
	prme();
}

int
doeat(void)
{
	struct obj *otmp;
	struct objclass *ftmp;
	int tmp;

	/* Is there some food (probably a heavy corpse) here on the ground? */
	if(!Levitation)
	for(otmp = fobj; otmp; otmp = otmp->nobj) {
		if(otmp->ox == u.ux && otmp->oy == u.uy &&
		   otmp->olet == FOOD_SYM) {
			pline("There %s %s here; eat %s? [ny] ",
				(otmp->quan == 1) ? "is" : "are",
				doname(otmp),
				(otmp->quan == 1) ? "it" : "one");
			if(readchar() == 'y') {
				if(otmp->quan != 1)
					splitobj(otmp, 1);
				freeobj(otmp);
				otmp = addinv(otmp);
				addtobill(otmp);
				goto gotit;
			}
		}
	}
	otmp = getobj("%", "eat");
	if(!otmp) return(0);
gotit:
	if(otmp->otyp == TIN){
		if(uwep) {
			switch(uwep->otyp) {
			case CAN_OPENER:
				tmp = 1;
				break;
			case DAGGER:
			case CRYSKNIFE:
				tmp = 3;
				break;
			case PICK_AXE:
			case AXE:
				tmp = 6;
				break;
			default:
				goto no_opener;
			}
			pline("Using your %s you try to open the tin.",
				aobjnam(uwep, NULL));
		} else {
		no_opener:
			pline("It is not so easy to open this tin.");
			if(Glib) {
				pline("The tin slips out of your hands.");
				if(otmp->quan > 1) {
					struct obj *obj;

					obj = splitobj(otmp, 1);
					if(otmp == uwep) setuwep(obj);
				}
				dropx(otmp);
				return(1);
			}
			tmp = 10 + rn2(1 + 500/((int)(u.ulevel + u.ustr)));
		}
		tin.reqtime = tmp;
		tin.usedtime = 0;
		tin.tin = otmp;
		occupation = opentin;
		occtxt = "opening the tin";
		return(1);
	}
	ftmp = &objects[otmp->otyp];
	multi = -ftmp->oc_delay;
	if(otmp->otyp >= CORPSE && eatcorpse(otmp)) goto eatx;
	if(!rn2(7) && otmp->otyp != FORTUNE_COOKIE) {
		pline("Blecch!  Rotten food!");
		if(!rn2(4)) {
			pline("You feel rather light headed.");
			Confusion += d(2,4);
		} else if(!rn2(4)&& !Blind) {
			pline("Everything suddenly goes dark.");
			Blind = d(2,10);
			seeoff(0);
		} else if(!rn2(3)) {
			if(Blind)
			  pline("The world spins and you slap against the floor.");
			else
			  pline("The world spins and goes dark.");
			nomul(-rnd(10));
			nomovemsg = "You are conscious again.";
		}
	} else {
		switch(otmp->otyp){
		case FOOD_RATION:
			pline("That food really hit the spot!");
			if(multi < 0) nomovemsg = "You finished your meal.";
			break;
		case TRIPE_RATION:
			pline("Yak - dog food!");
			more_experienced(1,0);
			flags.botl = 1;
			if(rn2(2)){
				pline("You vomit.");
				if(Sick) {
					Sick = 0;	/* David Neves */
					pline("What a relief!");
				}
			}
			break;
		default:
			if(otmp->otyp >= CORPSE)
			pline("That %s tasted terrible!",ftmp->oc_name);
			else
			pline("That %s was delicious!",ftmp->oc_name);
			if(otmp->otyp == DEAD_LIZARD && (Confusion > 2))
				Confusion = 2;
			else
#ifdef QUEST
			if(otmp->otyp == CARROT && !Blind){
				u.uhorizon++;
				setsee();
				pline("Your vision improves.");
			} else
#endif /* QUEST */
			if(otmp->otyp == FORTUNE_COOKIE) {
			  if(Blind) {
			    pline("This cookie has a scrap of paper inside!");
			    pline("What a pity, that you cannot read it!");
			  } else
			    outrumor();
			} else
			if(otmp->otyp == LUMP_OF_ROYAL_JELLY) {
				/* This stuff seems to be VERY healthy! */
				if(u.ustrmax < 118) u.ustrmax++;
				if(u.ustr < u.ustrmax) u.ustr++;
				u.uhp += rnd(20);
				if(u.uhp > u.uhpmax) {
					if(!rn2(17)) u.uhpmax++;
					u.uhp = u.uhpmax;
				}
				heal_legs();
			}
			break;
		}
	}
eatx:
	if(multi<0 && !nomovemsg){
		static char msgbuf[BUFSZ];
		sprintf(msgbuf, "You finished eating the %s.",
				ftmp->oc_name);
		nomovemsg = msgbuf;
	}
	useup(otmp);
	return(1);
}

#define	CORPSE_I_TO_C(otyp)	(char) ((otyp >= DEAD_ACID_BLOB)\
		     ?  'a' + (otyp - DEAD_ACID_BLOB)\
		     :	'@' + (otyp - DEAD_HUMAN))
bool
poisonous(struct obj *otmp)
{
	return(index(POISONOUS, CORPSE_I_TO_C(otmp->otyp)) != 0);
}

/* returns 1 if some text was printed */
static int
eatcorpse(struct obj *otmp)
{
char let = CORPSE_I_TO_C(otmp->otyp);
int tp = 0;
	if(let != 'a' && moves > otmp->age + 50 + rn2(100)) {
		tp++;
		pline("Ulch -- that meat was tainted!");
		pline("You get very sick.");
		Sick = 10 + rn2(10);
		u.usick_cause = objects[otmp->otyp].oc_name;
	} else if(index(POISONOUS, let) && rn2(5)){
		tp++;
		pline("Ecch -- that must have been poisonous!");
		if(!Poison_resistance){
			losestr(rnd(4));
			losehp(rnd(15), "poisonous corpse");
		} else
			pline("You don't seem affected by the poison.");
	} else if(index("ELNOPQRUuxz", let) && rn2(5)){
		tp++;
		pline("You feel sick.");
		losehp(rnd(8), "cadaver");
	}
	switch(let) {
	case 'L':
	case 'N':
	case 't':
		Teleportation |= INTRINSIC;
		break;
	case 'W':
		pluslvl();
		break;
	case 'n':
		u.uhp = u.uhpmax;
		flags.botl = 1;
		/* fall into next case */
	case '@':
		pline("You cannibal! You will be sorry for this!");
		/* not tp++; */
		/* fall into next case */
	case 'd':
		Aggravate_monster |= INTRINSIC;
		break;
	case 'I':
		if(!Invis) {
			Invis = 50+rn2(100);
			if(!See_invisible)
				newsym(u.ux, u.uy);
		} else {
			Invis |= INTRINSIC;
			See_invisible |= INTRINSIC;
		}
		/* fall into next case */
	case 'y':
#ifdef QUEST
		u.uhorizon++;
#endif /* QUEST */
		/* fall into next case */
	case 'B':
		Confusion = 50;
		break;
	case 'D':
		Fire_resistance |= INTRINSIC;
		break;
	case 'E':
		Telepat |= INTRINSIC;
		break;
	case 'F':
	case 'Y':
		Cold_resistance |= INTRINSIC;
		break;
	case 'k':
	case 's':
		Poison_resistance |= INTRINSIC;
		break;
	case 'c':
		pline("You turn to stone.");
		killer = "dead cockatrice";
		done("died");
		/* NOTREACHED */
	case 'a':
	  if(Stoned) {
	      pline("What a pity - you just destroyed a future piece of art!");
	      tp++;
	      Stoned = 0;
	  }
	  break;
	case 'M':
	  pline("You cannot resist the temptation to mimic a treasure chest.");
	  tp++;
	  nomul(-30);
	  afternmv = Meatdone;
	  nomovemsg = "You now again prefer mimicking a human.";
	  u.usym = '$';
	  prme();
	  break;
	}
	return(tp);
}
