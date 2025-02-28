/* NetHack 3.7	monattk.h	$NHDT-Date: 1596498548 2020/08/03 23:49:08 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.13 $ */
/* NetHack may be freely redistributed.  See license for details. */
/* Copyright 1988, M. Stephenson */

#ifndef MONATTK_H
#define MONATTK_H

/*	Add new attack types below - ordering affects experience (exper.c).
 *	Attacks > AT_BUTT are worth extra experience.
 */
#define AT_ANY (-1) /* fake attack; dmgtype_fromattack wildcard */
#define AT_NONE 0   /* passive monster (ex. acid blob) */
#define AT_CLAW 1   /* claw (punch, hit, etc.) */
#define AT_BITE 2   /* bite */
#define AT_KICK 3   /* kick */
#define AT_BUTT 4   /* head butt (ex. a unicorn) */
#define AT_TUCH 5   /* touches */
#define AT_STNG 6   /* sting */
#define AT_HUGS 7   /* crushing bearhug */
#define AT_SPIT 8  /* spits substance - ranged */
#define AT_ENGL 9  /* engulf (swallow or by a cloud) */
#define AT_BREA 10  /* breath - ranged */
#define AT_EXPL 11  /* explodes - proximity */
#define AT_BOOM 12  /* explodes when killed */
#define AT_GAZE 13  /* gaze - ranged */
#define AT_TENT 14  /* tentacles */
#define AT_SCRE 15  /* scream - sonic attack */
#define AT_VOLY 16  /* launch projectiles (ex. manticore spikes) */

#define AT_WEAP 17 /* uses weapon */
#define AT_MAGC 18 /* uses magic spell(s) */
#define LAST_AT AT_MAGC

/*	Add new damage types below.
 *
 *	Note that 1-10 correspond to the types of attack used in buzz().
 *	Please don't disturb the order unless you rewrite the buzz() code.
 */
#define AD_ANY (-1) /* fake damage; attacktype_fordmg wildcard */
#define AD_PHYS 0   /* ordinary physical */
#define AD_MAGM 1   /* magic missiles */
#define AD_FIRE 2   /* fire damage */
#define AD_COLD 3   /* frost damage */
#define AD_SLEE 4   /* sleep ray */
#define AD_DISN 5   /* disintegration (death ray) */
#define AD_ELEC 6   /* shock damage */
#define AD_DRST 7   /* drains str (poison) */
#define AD_ACID 8   /* acid damage */
#define AD_LOUD 9   /* previously for extension of buzz(), now for sonic */
#define AD_PSYC 10  /* previously for extension of buzz(), now for psychic */
#define AD_BLND 11  /* blinds (yellow light) */
#define AD_STUN 12  /* stuns */
#define AD_SLOW 13  /* slows */
#define AD_PLYS 14  /* paralyses */
#define AD_DRLI 15  /* drains life levels (Vampire) */
#define AD_DREN 16  /* drains magic energy */
#define AD_LEGS 17  /* damages legs (xan) */
#define AD_STON 18  /* petrifies (Medusa, cockatrice) */
#define AD_STCK 19  /* sticks to you (mimic) */
#define AD_SGLD 20  /* steals gold (leppie) */
#define AD_SITM 21  /* steals item (nymphs) */
#define AD_SEDU 22  /* seduces & steals multiple items */
#define AD_TLPT 23  /* teleports you (Quantum Mech.) */
#define AD_RUST 24  /* rusts armour (Rust Monster)*/
#define AD_CONF 25  /* confuses (Umber Hulk) */
#define AD_DGST 26  /* digests opponent (trapper, etc.) */
#define AD_HEAL 27  /* heals opponent's wounds (nurse) */
#define AD_WRAP 28  /* special "stick" for eels */
#define AD_WERE 29  /* confers lycanthropy */
#define AD_DRDX 30  /* drains dexterity (quasit) */
#define AD_DRCO 31  /* drains constitution */
#define AD_DRIN 32  /* drains intelligence (mind flayer) */
#define AD_DISE 33  /* confers diseases */
#define AD_DCAY 34  /* decays organics (brown Pudding) */
#define AD_SSEX 35  /* Succubus seduction (extended) */
#define AD_HALU 36  /* causes hallucination */
#define AD_DETH 37  /* for Death only */
#define AD_PEST 38  /* for Pestilence only */
#define AD_FAMN 39  /* for Famine only */
#define AD_SLIM 40  /* turns you into green slime */
#define AD_ENCH 41  /* remove enchantment (disenchanter) */
#define AD_CORR 42  /* corrode armor (black pudding) */
#define AD_VOID 43  /* disintegrate the target (void dragon) */
#define AD_MEMR 44  /* forget 1% of items (memory hound) */
#define AD_QUIL 45  /* quills and spikes (porcupine and manticore) */
#define AD_PITS 46  /* cause an earthquake (Lord Surtur) */
#define AD_CNCL 47  /* cancel the player (beholder) */
#define AD_DSRM 48  /* disarm the player */
#define AD_WIND 49  /* cause the defender to hurtle */
#define AD_CALM 50  /* calms down the defender (koala) */
#define AD_LUCK 51  /* damage the luck of the player (barghest) */
#define AD_HYDR 52  /* regrow heads (hydra) */
#define AD_MTRL 53  /* change material (transmuter) */
#define AD_LOST 54  /* get lost (yuki-onna) */
#define AD_TCKL 55	/* Tickle (Nightgaunts) */
#define AD_POLY 56  /* Polymorph (Genetic Engineer) */
#define AD_LARV 57  /* Inject Larvae (Brood Wasp) */
#define AD_SKEL 58  /* Generate skeletons (crawling cemetery) */
#define AD_HNGY 59 /* a mini version of FAMN, should fake a higher hunger rating instead  */
#define AD_POTN 60 /* potion effect (gel) */
#define AD_KDNP 61 /* kidnap (bar-lgura) */
#define AD_FUMB 62 /* fumbling (Kostchtchie) */
#define AD_CLOB 63 /* knockback */
#define AD_WTHR 64 /* withering (mummies) */
#define AD_LAWS 65 /* laws (lawyer) */
#define AD_VORP 66 /* beheading (vorpal jabberwock) */
#define AD_FEAR 67 /* inflict fear (eye of fear and flame) */

#define AD_CLRC 68 /* random clerical spell */
#define AD_SPEL 69 /* random magic spell */
#define AD_RBRE 70 /* random breath weapon */

#define AD_SAMU 71 /* hits, may steal Amulet (Wizard) */
#define AD_CURS 72 /* random curse (ex. gremlin) */
#define LAST_AD AD_CURS

struct mhitm_data {
    int damage;
    int hitflags; /* MM_DEF_DIED | MM_AGR_DIED | ... */
    boolean done;
    boolean permdmg;
    int specialdmg;
    int dieroll;
};

/*
 *  Monster to monster attacks.  When a monster attacks another (mattackm),
 *  any or all of the following can be returned.  See mattackm() for more
 *  details.
 */
#define MM_MISS 0x0     /* aggressor missed */
#define MM_HIT 0x1      /* aggressor hit defender */
#define MM_DEF_DIED 0x2 /* defender died */
#define MM_AGR_DIED 0x4 /* aggressor died */
#define MM_AGR_DONE 0x8 /* aggressor is done with their turn */

#endif /* MONATTK_H */
