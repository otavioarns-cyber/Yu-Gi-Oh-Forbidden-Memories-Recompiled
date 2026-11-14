Field Effects + Secondary Types v0.18 - Vanilla V2 test

Target base: Yu-Gi-Oh! Forbidden Memories Recompiled v0.2.1-preview.1.
The code mod remains API 4 compatible as required by the preview release.

Changes in this test build
- Fusion-derived Field identities added: Sword Arm of Dragon, Charubin the Fire Knight and Vermillion Sparrow gain Warrior; Lord of the Lamp gains Spellcaster.
- Aquatic inherited identities added: Kairyu-shin and Spike Seadra gain Aqua; Ice Water gains Fish. These identities are intentionally blocked from secondary Field modifiers while the current main type is Fish/Aqua/Sea Serpent.
- Sea Serpent is now represented explicitly in the secondary-type engine.
- Secondary Field resolution is set-like: maximum +200 or -200 per Field; positive and negative interactions on the same Field cancel.
- Secondary type equal to the current primary type is ignored for Field modifiers.
- Beast-Warrior blocks only Beast/Warrior secondary bonuses; unrelated subtypes remain functional.
- Current card type is read through the Recompiled card API, improving compatibility with other card/type-changing mods.
- Metal Fish and Mech Bass remove the current primary Machine Umi penalty intrinsically, then apply Fish secondary behavior.
- Generic footer now derives visible icons and Fields from the same rules used by gameplay. Blocked/cancelled-only interactions are omitted.
- Restored the terrain-card-delta-v1 provider used by compatible planners such as AI Hard Mode.
- v0.17 viewer lifetime guard is preserved.

Recommended validation
1. Sword Arm of Dragon: +200 ATK/DEF on Sogen from Warrior secondary type, plus its existing Dragon/Mountain secondary behavior.
2. Charubin the Fire Knight: +200 Sogen from Warrior. Its Pyro secondary identity is redundant with its current primary Pyro type, so it must not add a second Umi debuff.
3. Vermillion Sparrow: +200 Sogen from Warrior. Its Pyro secondary identity is redundant with its current primary Pyro type, so it must not add a second Umi debuff.
4. Lord of the Lamp: +200 Yami from Spellcaster.
5. Kairyu-shin / Spike Seadra / Ice Water: inherited Aqua/Fish identity must NOT add a second Umi/Wasteland modifier while their main type is aquatic; no blocked-only footer interaction should appear.
6. Summoned Skull footer: Viewed as [Thunder]/[Zombie], +200 ATK on Umi/Mountain/Yami/Wasteland.
7. Metal Fish and Mech Bass: +200 on Umi and -200 on Wasteland from Fish; the normal Machine -500 Umi penalty is removed and is not described in the footer.
8. A multi-subtype positive+negative collision on one Field must resolve to 0 and that Field must be omitted from the footer.
9. Open/close a card viewer during a duel, then finish the duel. Confirm no footer leaks into result/rank/reward screens.
10. Disable the MOD and confirm Vanilla behavior returns.
