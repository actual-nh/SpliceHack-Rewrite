/* NetHack 3.7	fountain.c	$NHDT-Date: 1596498170 2020/08/03 23:42:50 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.69 $ */
/*      Copyright Scott R. Turner, srt@ucla, 10/27/86 */
/* NetHack may be freely redistributed.  See license for details. */

/* Code for drinking from fountains. */

#include "hack.h"

static void dowatersnakes(void);
static void dowaterdemon(void);
static void dowaternymph(void);
static void gush(int, int, genericptr_t);
static void dofindgem(void);
static void blowupfurnace(int, int);

/* used when trying to dip in or drink from fountain or sink or pool while
   levitating above it, or when trying to move downwards in that state */
void
floating_above(const char *what)
{
    const char *umsg = "are floating high above the %s.";

    if (u.utrap && (u.utraptype == TT_INFLOOR || u.utraptype == TT_LAVA)) {
        /* when stuck in floor (not possible at fountain or sink location,
           so must be attempting to move down), override the usual message */
        umsg = "are trapped in the %s.";
        what = surface(u.ux, u.uy); /* probably redundant */
    }
    You(umsg, what);
}

/* Fountain of snakes! */
static void
dowatersnakes(void)
{
    register int num = rn1(5, 2);
    struct monst *mtmp;

    if (!(g.mvitals[PM_WATER_MOCCASIN].mvflags & G_GONE)) {
        if (!Blind)
            pline("An endless stream of %s pours forth!",
                  Hallucination ? makeplural(rndmonnam(NULL)) : "snakes");
        else
            You_hear("%s hissing!", something);
        while (num-- > 0)
            if ((mtmp = makemon(&mons[PM_WATER_MOCCASIN], u.ux, u.uy,
                                NO_MM_FLAGS)) != 0
                && t_at(mtmp->mx, mtmp->my))
                (void) mintrap(mtmp);
    } else
        pline_The("fountain bubbles furiously for a moment, then calms.");
}

/* Water demon */
static void
dowaterdemon(void)
{
    struct monst *mtmp;

    if (!(g.mvitals[PM_WATER_DEMON].mvflags & G_GONE)) {
        if ((mtmp = makemon(&mons[PM_WATER_DEMON], u.ux, u.uy,
                            NO_MM_FLAGS)) != 0) {
            if (!Blind)
                You("unleash %s!", a_monnam(mtmp));
            else
                You_feel("the presence of evil.");

            /* Give those on low levels a (slightly) better chance of survival
             */
            if (rnd(100) > (80 + level_difficulty())) {
                pline("Grateful for %s release, %s grants you a wish!",
                      mhis(mtmp), mhe(mtmp));
                /* give a wish and discard the monster (mtmp set to null) */
                mongrantswish(&mtmp);
            } else if (t_at(mtmp->mx, mtmp->my))
                (void) mintrap(mtmp);
        }
    } else
        pline_The("fountain bubbles furiously for a moment, then calms.");
}

/* Water Nymph */
static void
dowaternymph(void)
{
    register struct monst *mtmp;

    if (!(g.mvitals[PM_WATER_NYMPH].mvflags & G_GONE)
        && (mtmp = makemon(&mons[PM_WATER_NYMPH], u.ux, u.uy,
                           NO_MM_FLAGS)) != 0) {
        if (!Blind)
            You("attract %s!", a_monnam(mtmp));
        else
            You_hear("a seductive voice.");
        mtmp->msleeping = 0;
        if (t_at(mtmp->mx, mtmp->my))
            (void) mintrap(mtmp);
    } else if (!Blind)
        pline("A large bubble rises to the surface and pops.");
    else
        You_hear("a loud pop.");
}

/* Gushing forth along LOS from (u.ux, u.uy) */
void
dogushforth(int drinking)
{
    int madepool = 0;

    do_clear_area(u.ux, u.uy, 7, gush, (genericptr_t) &madepool);
    if (!madepool) {
        if (drinking)
            Your("thirst is quenched.");
        else
            pline("Water sprays all over you.");
    }
}

static void
gush(int x, int y, genericptr_t poolcnt)
{
    register struct monst *mtmp;
    register struct trap *ttmp;

    if (((x + y) % 2) || (x == u.ux && y == u.uy)
        || (rn2(1 + distmin(u.ux, u.uy, x, y))) || (levl[x][y].typ != ROOM)
        || (sobj_at(BOULDER, x, y)) || nexttodoor(x, y))
        return;

    if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
        return;

    if (!((*(int *) poolcnt)++))
        pline("Water gushes forth from the overflowing fountain!");

    /* Put a pool at x, y */
    levl[x][y].typ = POOL, levl[x][y].flags = 0;
    /* No kelp! */
    del_engr_at(x, y);
    water_damage_chain(g.level.objects[x][y], TRUE);

    if ((mtmp = m_at(x, y)) != 0)
        (void) minliquid(mtmp);
    else
        newsym(x, y);
}

/* Find a gem in the sparkling waters. */
static void
dofindgem(void)
{
    if (!Blind)
        You("spot a gem in the sparkling waters!");
    else
        You_feel("a gem here!");
    (void) mksobj_at(rnd_class(DILITHIUM_CRYSTAL, LUCKSTONE - 1), u.ux, u.uy,
                     FALSE, FALSE);
    SET_FOUNTAIN_LOOTED(u.ux, u.uy);
    newsym(u.ux, u.uy);
    exercise(A_WIS, TRUE); /* a discovery! */
}

void
dryup(xchar x, xchar y, boolean isyou)
{
    if (IS_FOUNTAIN(levl[x][y].typ)
        && (!rn2(3) || FOUNTAIN_IS_WARNED(x, y))) {
        if (isyou && in_town(x, y) && !FOUNTAIN_IS_WARNED(x, y)) {
            struct monst *mtmp;

            SET_FOUNTAIN_WARNED(x, y);
            /* Warn about future fountain use. */
            for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
                if (DEADMONSTER(mtmp))
                    continue;
                if (is_watch(mtmp->data) && couldsee(mtmp->mx, mtmp->my)
                    && mtmp->mpeaceful) {
                    if (!Deaf) {
                        pline("%s yells:", Amonnam(mtmp));
                        verbalize("Hey, stop using that fountain!");
                    } else {
                        pline("%s earnestly %s %s %s!",
                              Amonnam(mtmp),
                              nolimbs(mtmp->data) ? "shakes" : "waves",
                              mhis(mtmp),
                              nolimbs(mtmp->data)
                                      ? mbodypart(mtmp, HEAD)
                                      : makeplural(mbodypart(mtmp, ARM)));
                    }
                    break;
                }
            }
            /* You can see or hear this effect */
            if (!mtmp)
                pline_The("flow reduces to a trickle.");
            return;
        }
        if (isyou && wizard) {
            if (yn("Dry up fountain?") == 'n')
                return;
        }
        /* FIXME: sight-blocking clouds should use block_point() when
           being created and unblock_point() when going away, then this
           glyph hackery wouldn't be necessary */
        if (cansee(x, y)) {
            int glyph = glyph_at(x, y);

            if (!glyph_is_cmap(glyph) || glyph_to_cmap(glyph) != S_cloud)
                pline_The("fountain dries up!");
        }
        /* replace the fountain with ordinary floor */
        levl[x][y].typ = ROOM, levl[x][y].flags = 0;
        levl[x][y].blessedftn = 0;
        g.level.flags.nfountains--;
        /* The location is seen if the hero/monster is invisible
           or felt if the hero is blind. */
        newsym(x, y);
        if (isyou && in_town(x, y))
            (void) angry_guards(FALSE);
    }
}

void
dipfurnace(obj)
register struct obj *obj;
{
    burn_away_slime();
    switch(rnd(30)) {
        case 1:
        case 2:
        case 3:
            bless(obj);
            break;
        case 4:
            if (!Blind)
                pline("%s flickers with purple light.", Doname2(obj));
            else
                pline("%s shudders slightly in your grip.", Doname2(obj));
            obj->oerodeproof = 1;
            break;
        case 5:
            blowupfurnace(u.ux, u.uy);
            break;
        case 6: /* Strange feeling */
            pline("An unsettling tingling runs up your %s.", body_part(ARM));
            break;
        case 7: /* Strange feeling */
            You_feel("a sudden flare of heat.");
            break;
        default:
            pline("The lava in the furnace bubbles.");
            break;
    }
    lava_damage(obj, u.ux, u.uy);
    update_inventory();
}

void
drinkfountain(void)
{
    /* What happens when you drink from a fountain? */
    register boolean mgkftn = (levl[u.ux][u.uy].blessedftn == 1);
    register int fate = rnd(30);

    if (Levitation) {
        floating_above("fountain");
        return;
    }

    if (mgkftn && u.uluck >= 0 && fate >= 10) {
        int i, ii, littleluck = (u.uluck < 4);

        pline("Wow!  This makes you feel great!");
        /* blessed restore ability */
        for (ii = 0; ii < A_MAX; ii++)
            if (ABASE(ii) < AMAX(ii)) {
                ABASE(ii) = AMAX(ii);
                g.context.botl = 1;
            }
        /* gain ability, blessed if "natural" luck is high */
        i = rn2(A_MAX); /* start at a random attribute */
        for (ii = 0; ii < A_MAX; ii++) {
            if (adjattrib(i, 1, littleluck ? -1 : 0) && littleluck)
                break;
            if (++i >= A_MAX)
                i = 0;
        }
        display_nhwindow(WIN_MESSAGE, FALSE);
        pline("A wisp of vapor escapes the fountain...");
        exercise(A_WIS, TRUE);
        levl[u.ux][u.uy].blessedftn = 0;
        return;
    }

    if (fate < 10) {
        pline_The("cool draught refreshes you.");
        u.uhunger += rnd(10); /* don't choke on water */
        newuhs(FALSE);
        if (mgkftn)
            return;
    } else {
        switch (fate) {
        case 19: /* Self-knowledge */
            You_feel("self-knowledgeable...");
            display_nhwindow(WIN_MESSAGE, FALSE);
            enlightenment(MAGICENLIGHTENMENT, ENL_GAMEINPROGRESS);
            exercise(A_WIS, TRUE);
            pline_The("feeling subsides.");
            break;
        case 20: /* Foul water */
            pline_The("water is foul!  You gag and vomit.");
            morehungry(rn1(20, 11));
            vomit();
            break;
        case 21: /* Poisonous */
            pline_The("water is contaminated!");
            if (Poison_resistance) {
                pline("Perhaps it is runoff from the nearby %s farm.",
                      fruitname(FALSE));
                losehp(rnd(4), "unrefrigerated sip of juice", KILLED_BY_AN);
                break;
            }
            losestr(rn1(4, 3));
            losehp(rnd(10), "contaminated water", KILLED_BY);
            exercise(A_CON, FALSE);
            break;
        case 22: /* Fountain of snakes! */
            dowatersnakes();
            break;
        case 23: /* Water demon */
            dowaterdemon();
            break;
        case 24: { /* Maybe curse some items */
            register struct obj *obj;
            int buc_changed = 0;

            pline("This water's no good!");
            morehungry(rn1(20, 11));
            exercise(A_CON, FALSE);
            /* this is more severe than rndcurse() */
            for (obj = g.invent; obj; obj = obj->nobj)
                if (obj->oclass != COIN_CLASS && !obj->cursed && !rn2(5)) {
                    curse(obj);
                    ++buc_changed;
                }
            if (buc_changed)
                update_inventory();
            break;
        }
        case 25: /* See invisible */
            if (Blind) {
                if (Invisible) {
                    You("feel transparent.");
                } else {
                    You("feel very self-conscious.");
                    pline("Then it passes.");
                }
            } else {
                You_see("an image of someone stalking you.");
                pline("But it disappears.");
            }
            HSee_invisible |= FROMOUTSIDE;
            newsym(u.ux, u.uy);
            exercise(A_WIS, TRUE);
            break;
        case 26: /* See Monsters */
            (void) monster_detect((struct obj *) 0, 0);
            exercise(A_WIS, TRUE);
            break;
        case 27: /* Find a gem in the sparkling waters. */
            if (!FOUNTAIN_IS_LOOTED(u.ux, u.uy)) {
                dofindgem();
                break;
            }
            /*FALLTHRU*/
        case 28: /* Water Nymph */
            dowaternymph();
            break;
        case 29: /* Scare */
        {
            register struct monst *mtmp;

            pline("This %s gives you bad breath!",
                  hliquid("water"));
            for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
                if (DEADMONSTER(mtmp))
                    continue;
                monflee(mtmp, 0, FALSE, FALSE);
            }
            break;
        }
        case 30: /* Gushing forth in this room */
            dogushforth(TRUE);
            break;
        default:
            pline("This tepid %s is tasteless.",
                  hliquid("water"));
            break;
        }
    }
    dryup(u.ux, u.uy, TRUE);
}

/* Monty Python and the Holy Grail ;) */
static const char *const excalmsgs[] = {
    "had Excalibur thrown at them by some watery tart",
    "received Excalibur from a strange woman laying about in a pond",
    "signified by divine providence, was chosen to carry Excalibur",
    "has been given Excalibur, and now enjoys supreme executive power",
    "endured an absurd aquatic ceremony, and now wields Excalibur"
};

void
dipfountain(register struct obj *obj)
{
    if (Levitation) {
        floating_above("fountain");
        return;
    }

    /* Don't grant Excalibur when there's more than one object.  */
    /* (quantity could be > 1 if merged daggers got polymorphed) */
    if (obj->otyp == LONG_SWORD && obj->quan == 1L && u.ulevel >= 5 && !rn2(6)
        && !obj->oartifact
        && !exist_artifact(LONG_SWORD, artiname(ART_EXCALIBUR))) {
        if (u.ualign.type != A_LAWFUL) {
            /* Ha!  Trying to cheat her. */
            pline("A freezing mist rises from the %s and envelopes the sword.",
                  hliquid("water"));
            pline_The("fountain disappears!");
            curse(obj);
            if (obj->spe > -6 && !rn2(3))
                obj->spe--;
            obj->oerodeproof = FALSE;
            exercise(A_WIS, FALSE);
            livelog_printf(LL_ARTIFACT, "was denied Excalibur! The Lady of the Lake has deemed %s unworthy", uhim());
        } else {
            /* The lady of the lake acts! - Eric Backus */
            /* Be *REAL* nice */
            pline(
              "From the murky depths, a hand reaches up to bless the sword.");
            pline("As the hand retreats, the fountain disappears!");
            obj = oname(obj, artiname(ART_EXCALIBUR));
            discover_artifact(ART_EXCALIBUR);
            bless(obj);
            obj->oeroded = obj->oeroded2 = 0;
            obj->oerodeproof = TRUE;
            exercise(A_WIS, TRUE);
            livelog_printf(LL_ARTIFACT, "%s", excalmsgs[rn2(SIZE(excalmsgs))]);
        }
        update_inventory();
        levl[u.ux][u.uy].typ = ROOM, levl[u.ux][u.uy].flags = 0;
        newsym(u.ux, u.uy);
        g.level.flags.nfountains--;
        if (in_town(u.ux, u.uy))
            (void) angry_guards(FALSE);
        return;
    } else {
        int er = water_damage(obj, NULL, TRUE);

        if (obj->otyp == POT_ACID
            && er != ER_DESTROYED) { /* Acid and water don't mix */
            useup(obj);
            return;
        } else if (er != ER_NOTHING && !rn2(2)) { /* no further effect */
            return;
        }
    }

    switch (rnd(30)) {
    case 16: /* Curse the item */
        if (obj->oclass != COIN_CLASS && !obj->cursed) {
            curse(obj);
        }
        break;
    case 17:
    case 18:
    case 19:
    case 20: /* Uncurse the item */
        if (obj->cursed) {
            if (!Blind)
                pline_The("%s glows for a moment.", hliquid("water"));
            uncurse(obj);
        } else {
            pline("A feeling of loss comes over you.");
        }
        break;
    case 21: /* Water Demon */
        dowaterdemon();
        break;
    case 22: /* Water Nymph */
        dowaternymph();
        break;
    case 23: /* an Endless Stream of Snakes */
        dowatersnakes();
        break;
    case 24: /* Find a gem */
        if (!FOUNTAIN_IS_LOOTED(u.ux, u.uy)) {
            dofindgem();
            break;
        }
        /*FALLTHRU*/
    case 25: /* Water gushes forth */
        dogushforth(FALSE);
        break;
    case 26: /* Strange feeling */
        pline("A strange tingling runs up your %s.", body_part(ARM));
        break;
    case 27: /* Strange feeling */
        You_feel("a sudden chill.");
        break;
    case 28: /* Strange feeling */
        pline("An urge to take a bath overwhelms you.");
        {
            long money = money_cnt(g.invent);
            struct obj *otmp;
            if (money > 10) {
                /* Amount to lose.  Might get rounded up as fountains don't
                 * pay change... */
                money = somegold(money) / 10;
                for (otmp = g.invent; otmp && money > 0; otmp = otmp->nobj)
                    if (otmp->oclass == COIN_CLASS) {
                        int denomination = objects[otmp->otyp].oc_cost;
                        long coin_loss =
                            (money + denomination - 1) / denomination;
                        coin_loss = min(coin_loss, otmp->quan);
                        otmp->quan -= coin_loss;
                        money -= coin_loss * denomination;
                        if (!otmp->quan)
                            delobj(otmp);
                    }
                You("lost some of your gold in the fountain!");
                CLEAR_FOUNTAIN_LOOTED(u.ux, u.uy);
                exercise(A_WIS, FALSE);
            }
        }
        break;
    case 29: /* You see coins */
        /* We make fountains have more coins the closer you are to the
         * surface.  After all, there will have been more people going
         * by.  Just like a shopping mall!  Chris Woodbury  */

        if (FOUNTAIN_IS_LOOTED(u.ux, u.uy))
            break;
        SET_FOUNTAIN_LOOTED(u.ux, u.uy);
        (void) mkgold((long) (rnd((dunlevs_in_dungeon(&u.uz) - dunlev(&u.uz)
                                   + 1) * 2) + 5),
                      u.ux, u.uy);
        if (!Blind)
            pline("Far below you, you see coins glistening in the %s.",
                  hliquid("water"));
        exercise(A_WIS, TRUE);
        newsym(u.ux, u.uy);
        break;
    }
    update_inventory();
    dryup(u.ux, u.uy, TRUE);
}

void
breakfurnace(x, y)
int x, y;
{
    if (cansee(x, y) || (x == u.ux && y == u.uy))
        pline_The("furnace breaks apart and spills its contents!");
    levl[x][y].typ = LAVAPOOL, levl[x][y].flags = 0;
    newsym(x, y);
    g.level.flags.nfurnaces--;
}

void
blowupfurnace(x, y)
int x, y;
{
    if (cansee(x, y) || (x == u.ux && y == u.uy))
        pline_The("furnace rumbles, then explodes into smithereens!");
    levl[x][y].typ = ROOM, levl[x][y].flags = 0;
    newsym(x, y);
    g.level.flags.nfurnaces--;
    explode(u.ux, u.uy, 11, rnd(30), TOOL_CLASS, EXPL_FIERY);
}

void
drinkfurnace(void)
{
    if (Levitation) {
        floating_above("furnace");
        return;
    }
    if (!likes_lava(g.youmonst.data)) {
        pline("Glug glug glug...");
        /* ugly hack, look away */
        u.uhp = 0;
        losehp(1, "trying to drink lava", KILLED_BY);
        return;
    }
    burn_away_slime();
    switch(rn2(20)) {
    case 0:
        pline("You chug some lava. Yum!");
        u.uhunger += rnd(50);
        break;
    case 1:
        breakfurnace(u.ux, u.uy);
        break;
    case 2:
    case 3:
        pline_The("%s moves as though of its own will!", hliquid("lava"));
        if ((g.mvitals[PM_LAVA_DEMON].mvflags & G_GONE)
            || !makemon(&mons[PM_LAVA_DEMON], u.ux, u.uy, NO_MM_FLAGS))
            pline("But it settles down.");
        break;
    default:
        pline("You take a sip of the lava.");
        u.uhunger += rnd(5);
    }
}

static const short fusions[][3] = {
    { IRON_CHAIN, KNIFE, SPIKED_CHAIN },
    { IRON_CHAIN, DAGGER, SPIKED_CHAIN },
    { AMULET_OF_REFLECTION, LARGE_SHIELD, SHIELD_OF_REFLECTION },
    { FLAIL, FLAIL, TRIPLE_FLAIL },
    { AXE, AXE, BATTLE_AXE },
    { DENTED_POT, DUNCE_CAP, TINFOIL_HAT },
    { RIN_HUNGER, RIN_AGGRAVATE_MONSTER, RIN_CONFLICT },
    { RIN_TELEPORTATION, RIN_POLYMORPH_CONTROL, RIN_TELEPORT_CONTROL },
    { RIN_POLYMORPH, RIN_TELEPORT_CONTROL, RIN_POLYMORPH_CONTROL },
    { WAN_OPENING, WAN_LOCKING, WAN_NOTHING },
    { IRON_CHAIN, BOULDER, HEAVY_IRON_BALL },
    { C_RATION, K_RATION, FOOD_RATION },
    { FOOD_RATION, TOOLED_HORN, HORN_OF_PLENTY },
    { AMULET_OF_ESP, HELMET, HELM_OF_TELEPATHY },
    { CLUB, RUBBER_HOSE, BASEBALL_BAT },
    { 0, 0, 0, }
    /* scale mail + dragon scales = dsm */
};

int
doforging(void)
{
    struct obj* obj1;
    struct obj* obj2;
    boolean combi_done = FALSE;
    int arti_id = 0;
    int i;
    if (!IS_FURNACE(levl[u.ux][u.uy].typ)) {
        pline("You need a forge in order to forge items.");
        return 1;
    }
    obj1 = getobj("use as a base", any_obj_ok, GETOBJ_PROMPT);
    obj2 = getobj("combine with the base item", any_obj_ok, GETOBJ_PROMPT);
    if (obj1 == obj2) {
        pline("You cannot combine an item with itself!");
        return 1;
    }
    if (!obj1 && !obj2) {
        pline("You need more than one object.");
        return 1;
    }
    if (obj2->oartifact && !obj1->oartifact) {
        obj1 = obj2;
    }
    /* Artifact fusions. */
    if (obj1->oartifact && obj2->oartifact) {
        if (obj1->oartifact == ART_FROST_BRAND && obj2->oartifact == ART_FIRE_BRAND
            || obj1->oartifact == ART_FIRE_BRAND && obj2->oartifact == ART_FROST_BRAND) {
            arti_id = ART_FROSTBURN;
            combi_done = TRUE;
        }
        if (combi_done) {
            obj1->oartifact = 0;
            obj1 = oname(obj1, artiname(arti_id));
            pline("You create %s!", an(xname(obj1)));
            livelog_printf(LL_DIVINEGIFT,
                        "used a forge to create %s", an(xname(obj1)));
            update_inventory();
        } else {
            pline("The artifact refuses to be combined.");
            return 1;
        }
    }
    /* Mundane item fusions */
    if (!combi_done) {
        for (int i = 0; fusions[i][0] > 0; i++) {
            if ((obj1->otyp == fusions[i][0] && obj2->otyp == fusions[i][1]) ||
                (obj2->otyp == fusions[i][0] && obj1->otyp == fusions[i][1])) {
                obj1->otyp = fusions[i][2];
                break;
            }
        }
    }
    /* Take on the secondary object's material. */
    if (valid_obj_material(obj1, obj2->material)) obj1->material = obj2->material;
    /* Use whichever enchantment is higher. */
    if (obj2->spe > obj1->spe) obj1->spe = min(obj2->spe, 10);
    /* Keep curses around. */
    if (obj2->cursed) obj1->cursed = TRUE;
    useup(obj2);
    /* Print a message. */
    if (!combi_done) pline("You combine the items in the furnace.");
    /* Destroy the furnace. */
    pline("The lava in the furnace cools.");
    levl[u.ux][u.uy].typ = ROOM;
    newsym(u.ux, u.uy);
    g.level.flags.nfurnaces--;
    return 0;
}

void
breaksink(int x, int y)
{
    if (cansee(x, y) || (x == u.ux && y == u.uy))
        pline_The("pipes break!  Water spurts out!");
    g.level.flags.nsinks--;
    levl[x][y].typ = FOUNTAIN, levl[x][y].looted = 0;
    levl[x][y].blessedftn = 0;
    SET_FOUNTAIN_LOOTED(x, y);
    g.level.flags.nfountains++;
    newsym(x, y);
}

void
drinksink(void)
{
    struct obj *otmp;
    struct monst *mtmp;

    if (Levitation) {
        floating_above("sink");
        return;
    }
    switch (rn2(20)) {
    case 0:
        You("take a sip of very cold %s.", hliquid("water"));
        break;
    case 1:
        You("take a sip of very warm %s.", hliquid("water"));
        break;
    case 2:
        You("take a sip of scalding hot %s.", hliquid("water"));
        if (Fire_resistance) {
            pline("It seems quite tasty.");
            monstseesu(M_SEEN_FIRE);
        } else
            losehp(rnd(6), "sipping boiling water", KILLED_BY);
        /* boiling water burns considered fire damage */
        break;
    case 3:
        if (g.mvitals[PM_SEWER_RAT].mvflags & G_GONE)
            pline_The("sink seems quite dirty.");
        else {
            mtmp = makemon(&mons[PM_SEWER_RAT], u.ux, u.uy, NO_MM_FLAGS);
            if (mtmp)
                pline("Eek!  There's %s in the sink!",
                      (Blind || !canspotmon(mtmp)) ? "something squirmy"
                                                   : a_monnam(mtmp));
        }
        break;
    case 4:
        do {
            otmp = mkobj(POTION_CLASS, FALSE);
            if (otmp->otyp == POT_WATER) {
                obfree(otmp, (struct obj *) 0);
                otmp = (struct obj *) 0;
            }
        } while (!otmp);
        otmp->cursed = otmp->blessed = 0;
        pline("Some %s liquid flows from the faucet.",
              Blind ? "odd" : hcolor(OBJ_DESCR(objects[otmp->otyp])));
        otmp->dknown = !(Blind || Hallucination);
        otmp->quan++;       /* Avoid panic upon useup() */
        otmp->fromsink = 1; /* kludge for docall() */
        (void) dopotion(otmp);
        obfree(otmp, (struct obj *) 0);
        break;
    case 5:
        if (!(levl[u.ux][u.uy].looted & S_LRING)) {
            You("find a ring in the sink!");
            (void) mkobj_at(RING_CLASS, u.ux, u.uy, TRUE);
            levl[u.ux][u.uy].looted |= S_LRING;
            exercise(A_WIS, TRUE);
            newsym(u.ux, u.uy);
        } else
            pline("Some dirty %s backs up in the drain.", hliquid("water"));
        break;
    case 6:
        breaksink(u.ux, u.uy);
        break;
    case 7:
        pline_The("%s moves as though of its own will!", hliquid("water"));
        if ((g.mvitals[PM_WATER_ELEMENTAL].mvflags & G_GONE)
            || !makemon(&mons[PM_WATER_ELEMENTAL], u.ux, u.uy, NO_MM_FLAGS))
            pline("But it quiets down.");
        break;
    case 8:
        pline("Yuk, this %s tastes awful.", hliquid("water"));
        more_experienced(1, 0);
        newexplevel();
        break;
    case 9:
        pline("Gaggg... this tastes like sewage!  You vomit.");
        morehungry(rn1(30 - ACURR(A_CON), 11));
        vomit();
        break;
    case 10:
        pline("This %s contains toxic wastes!", hliquid("water"));
        if (!Unchanging) {
            You("undergo a freakish metamorphosis!");
            polyself(0);
        }
        break;
    /* more odd messages --JJB */
    case 11:
        You_hear("clanking from the pipes...");
        break;
    case 12:
        You_hear("snatches of song from among the sewers...");
        break;
    case 19:
        if (Hallucination) {
            pline("From the murky drain, a hand reaches up... --oops--");
            break;
        }
        /*FALLTHRU*/
    default:
        You("take a sip of %s %s.",
            rn2(3) ? (rn2(2) ? "cold" : "warm") : "hot",
            hliquid("water"));
    }
}

/*fountain.c*/
