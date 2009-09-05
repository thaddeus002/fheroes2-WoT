#include <string>
#include <algorithm>
#include <cassert>
#include "battle.h"
#include "battle_types.h"
#include "engine.h"
#include "agg.h"
#include "heroes.h"
#include "settings.h"
#include "button.h"
#include "cursor.h"
#include "portrait.h"
#include "morale.h"
#include "luck.h"

#define display Display::Get()
#define cursor Cursor::Get()
#define le LocalEvent::Get()

static void DrawArmySummary(const Army::BattleArmy_t &orig, const Army::BattleArmy_t &current, const Rect &draw)
{
    if(orig.size() != current.size())
    {
        Error::Warning("except: orig.size() != current.size()");
        return;
    }

    Army::army_t killed;

    for(u8 i = 0; i < orig.size(); i++)
	if(orig[i].isValid() && orig[i].Count() >= current[i].Count())
	    killed.At(i).Set(orig[i], orig[i].Count() - current[i].Count());

    if(killed.isValid())
	killed.DrawMons32Line(draw.x, draw.y, draw.w);
    else
    {
        Text none(_("None"), Font::SMALL);
        none.Blit(draw.x + (draw.w - none.w()) / 2, draw.y + 15);
    }
}

static bool PredicateCanTakeArtifact(const Artifact & artifact)
{
    return artifact.isValid() && artifact() != Artifact::MAGIC_BOOK;
}

void Battle::BattleSummary(HeroBase &hero, u32 exp, const Army::ArmyPairs &armies,
                         const BagArtifacts *artifacts, Spell::spell_t spell,
                         u32 deadRaised, Army::battle_t status)
{
    if(BattleSettings::Get().Modes(BattleSettings::OPT_LOGICONLY))
        return;

    AGG::PlayMusic(status == Army::WIN ? MUS::BATTLEWIN : MUS::BATTLELOSE);

    const std::string &name = hero.GetName();
    ICN::icn_t interface = Settings::Get().EvilInterface() ? ICN::WINLOSEE : ICN::WINLOSE;
    ICN::icn_t buttonIcon = Settings::Get().EvilInterface() ? ICN::WINCMBBE : ICN::WINCMBTB;
    const Sprite &background = AGG::GetICN(interface, 0);
    const Sprite &shadow = AGG::GetICN(interface, 1);
    const Sprite &button = AGG::GetICN(buttonIcon, 0);
    const int backgroundX = (display.w() - background.w()) / 2;
    const int backgroundY = (display.h() - background.h()) / 2;
    Button buttonDone(backgroundX + (background.w() - button.w()) / 2 + 1, backgroundY + background.h() - button.h() * 2 + 3, buttonIcon, 0, 1);
    
    std::string message[2];
    std::vector<std::pair<ICN::icn_t, Point> > animation;
    switch(status)
    {
        case Army::WIN:
            animation.push_back(std::make_pair(ICN::WINCMBT, Point(32, 0)));
            message[0] = _("A glorious victory!");
            message[1] = _("For valor in combat, %{name} receives %{exp} experience.");
            String::Replace(message[1], "%{name}", name);
            String::Replace(message[1], "%{exp}", exp);
            break;
        case Army::LOSE:
            animation.push_back(std::make_pair(ICN::CMBTLOS1, Point(0, 0)));
            animation.push_back(std::make_pair(ICN::CMBTLOS2, Point(0, 0)));
            animation.push_back(std::make_pair(ICN::CMBTLOS3, Point(0, 29)));
            message[0] = _("Your forces suffer a bitter defeat, and %{name} abandons your cause.");
            String::Replace(message[0], "%{name}", name);
            break;
        case Army::SURRENDER:
            animation.push_back(std::make_pair(ICN::CMBTSURR, Point(32, 3)));
            message[0] = _("%{name} surrenders to the enemy, and departs in shame.");
            String::Replace(message[0], "%{name}", name);
            break;
        case Army::RETREAT:
            animation.push_back(std::make_pair(ICN::CMBTFLE1, Point(34, 42)));
            animation.push_back(std::make_pair(ICN::CMBTFLE2, Point(0, 12)));
            animation.push_back(std::make_pair(ICN::CMBTFLE3, Point(0, 0)));
            message[0] = _("Your forces in disarray, the cowardly %{name} flees in terror.");
            String::Replace(message[0], "%{name}", name);
            break;
        default:
            assert(0 && "Invalid battle state.");
            break;
    }
    
    const Sprite &animBase = AGG::GetICN(animation[0].first, 0, false);
    const int baseAnimX = backgroundX + (background.w() - animBase.w()) / 2 - 1;
    const int baseAnimY = backgroundY + 36;
   
    cursor.SetThemes(cursor.WAR_POINTER); 
    display.Fade(100);
    display.Flip();
    cursor.Hide();
    display.Blit(shadow, backgroundX - 17, backgroundY + 10);
    display.Blit(background, backgroundX, backgroundY);
    display.Blit(animBase, baseAnimX, baseAnimY);
    buttonDone.Draw();

    TextBox first(message[0], Font::BIG, Rect(baseAnimX - 20, backgroundY + 180, animBase.w() + 40, background.h() - 160));
    if(message[1].size())
	TextBox::TextBox(message[1], Font::BIG, Rect(baseAnimX - 20, first.y() + first.h(),
            animBase.w() + 40, background.h() - 160 - first.h()));

    const int textY = backgroundY + background.h() / 2 + 30;
    TextBox title(_("Battlefield Casualties"), Font::SMALL, Rect(backgroundX, textY, background.w(), 40));
    TextBox attacker(_("Attacker"), Font::SMALL, Rect(backgroundX, title.y() + int(title.h() * 1.5f), background.w(), 40));
    DrawArmySummary(*armies[0].second, *armies[0].first, Rect(backgroundX + 80, attacker.y() + attacker.h() + 10, 150, 0));
    TextBox defender(_("Defender"), Font::SMALL, Rect(backgroundX, attacker.y() + attacker.h() + 55, background.w(), 40));
    DrawArmySummary(*armies[1].second, *armies[1].first, Rect(backgroundX + 80, defender.y() + defender.h() + 10, 150, 0));

    cursor.Show();
    display.Flip();
    
    u32 ticket = 0;
    u32 frame = 1, subanim = 0, count = AGG::GetICNCount(animation[subanim].first);
    u16 artIndex = 0;
    if(artifacts)
    {
        BagArtifacts::const_iterator artifact = std::find_if(artifacts->begin(), artifacts->end(),
                                                             PredicateCanTakeArtifact);
        if(artifact != artifacts->end())
            artIndex = artifact - artifacts->begin();
        else artIndex = MAXU16;
    }
    
    int state = 0;
    while(le.HandleEvents()) {
        if(le.MousePressLeft(buttonDone)) buttonDone.PressDraw();
        else buttonDone.ReleaseDraw();
        
        if(le.KeyPress(KEY_RETURN) || le.MouseClickLeft(buttonDone))
        {
            cursor.Hide();
            bool done = false;
            switch(state)
            {
                case 0:
                    state++; //fall-through
                case 1:
                    if(!artifacts)
                        state++; //fall-through
                    else if(artIndex < artifacts->size())
                    {
                        display.Blit(background, backgroundX, backgroundY);
                        AGG::PlaySound(M82::PICKUP01);
                        const Sprite & art = AGG::GetICN(ICN::ARTIFACT, artifacts->at(artIndex).GetIndexSprite64());
                        const Sprite & border = AGG::GetICN(ICN::WINLOSEB, 0);
                        const std::string & artName = artifacts->at(artIndex).GetName();
                        Rect titleRect(backgroundX + background.w() / 8, baseAnimY + animBase.h() + 20, background.w() * 3 / 4, background.h());
                        TextBox title(_("You have captured an enemy artifact!"), Font::BIG, titleRect);
                        Text artText(artName, Font::SMALL);
                        artText.Blit(backgroundX + (background.w() - artText.w()) / 2, buttonDone.y - artText.h() - 5);
                        int artX = backgroundX + (background.w() - art.w()) / 2;
                        int artY = buttonDone.y - artText.h() - 10 - art.h() - 10;
                        display.Blit(art, artX, artY);
                        display.Blit(border, artX - (border.w() - art.w()) / 2, artY - (border.h() - art.h()) / 2);

                        //Scan for the next valid artifact in the list
                        while(++artIndex < artifacts->size() &&
                              !PredicateCanTakeArtifact(artifacts->at(artIndex))) ;

                        if(artIndex == artifacts->size())
                            state++;
                        break;
                    }
                    else state++; //fall-through
                case 2:
                {
                    state++; //fall-through
                    if(spell != Spell::NONE)
                    {
                        //TODO: Eagle eye text
                        display.Blit(background, backgroundX, backgroundY);
                        const Sprite &spr = AGG::GetICN(ICN::SPELLS, Spell::IndexSprite(spell));
                        display.Blit(spr, backgroundX + (background.w() - spr.w()) / 2, backgroundY + (background.h() - spr.h()) / 2);
                        AGG::PlaySound(M82::PICKUP01);
                        if(hero.GetSpellBook().isActive())
                            hero.GetSpellBook().Append(spell, hero.GetLevelSkill(Skill::Secondary::WISDOM));
                        break;
                    }
                }
                case 3:
                    state++; //fall-through
                    if(deadRaised != 0)
                    {
                        //TODO: Necromancy text
                        Monster monster(Monster::SKELETON);
                        const Sprite &spr = AGG::GetICN(ICN::MONS32, monster.GetSpriteIndex());
                        display.Blit(background, backgroundX, backgroundY);
                        display.Blit(spr, backgroundX + (background.w() - spr.w()) / 2, backgroundY + (background.h() - spr.h()) / 2);
                        AGG::PlaySound(M82::PICKUP01);
                        hero.GetArmy().JoinTroop(Monster::SKELETON, deadRaised);
                        break;
                    }
                case 4:
                    done = true;
                    break;
            }
            if(done)
              break;
            cursor.Show();
            display.Flip();
        }
        
        if(frame < count && Game::ShouldAnimateInfrequent(ticket++, 3))
        {
            const Sprite &animFrame = AGG::GetICN(animation[subanim].first, frame++, false);
            cursor.Hide();
            display.Blit(AGG::GetICN(animation[subanim].first, 0), baseAnimX, baseAnimY);
            display.Blit(animFrame, animation[subanim].second.x + baseAnimX, animation[subanim].second.y + baseAnimY);
            cursor.Show();
            display.Flip();
            if(frame == count && animation[subanim].first != ICN::CMBTFLE3)
            {
                if(subanim < animation.size() - 1)
                    count = AGG::GetICNCount(animation[++subanim].first);
                frame = 1;
            }
        }
    }
}

void Battle::SettingsDialog()
{ 
    const ICN::icn_t sett = Settings::Get().EvilInterface() ? ICN::CSPANBKE : ICN::CSPANBKG;
    const ICN::icn_t butt = Settings::Get().EvilInterface() ? ICN::CSPANBTE : ICN::CSPANBTN;
    const Surface & dialog = AGG::GetICN(sett, 0);

    Rect pos_rt;
    pos_rt.x = (display.w() - dialog.w()) / 2;
    pos_rt.y = (display.h() - dialog.h()) / 2;
    pos_rt.w = dialog.w();
    pos_rt.h = dialog.h();

    cursor.Hide();

    Background back(pos_rt);
    back.Save();

    display.Blit(dialog, pos_rt.x, pos_rt.y);
    
    // 0-2 speed
    // 3-5 monster info
    // 6,7 auto spell
    // 8,9 grid
    // 10,11 shadow movement
    // 12,13 shadow cursor
    Button buttonOK(pos_rt.x + 113, pos_rt.y + 252, butt, 0, 1);
    Button optGrid(pos_rt.x + 36, pos_rt.y + 157, ICN::CSPANEL, 8, 9);
    Button optSM(pos_rt.x + 128, pos_rt.y + 157, ICN::CSPANEL, 10, 11);
    Button optSC(pos_rt.x + 220, pos_rt.y + 157, ICN::CSPANEL, 12, 13);
    buttonOK.Draw();
    optGrid.Draw();
    optSM.Draw();
    optSC.Draw();
    if(BattleSettings::Get().Modes(BattleSettings::OPT_GRID)) optGrid.PressDraw();
    if(BattleSettings::Get().Modes(BattleSettings::OPT_MOVESHADOW)) optSM.PressDraw();
    if(BattleSettings::Get().Modes(BattleSettings::OPT_MOUSESHADOW)) optSC.PressDraw();

    display.Flip();
    cursor.Show();

    // dialog menu loop
    while(le.HandleEvents()) {
        // exit
        if(le.KeyPress(KEY_ESCAPE))
            return;
        do_button(buttonOK, return, {});
        do_rbutton(optGrid, BattleSettings::Get().ToggleModes(BattleSettings::OPT_GRID), {});
        do_rbutton(optSM, BattleSettings::Get().ToggleModes(BattleSettings::OPT_MOVESHADOW), {});
        do_rbutton(optSC, BattleSettings::Get().ToggleModes(BattleSettings::OPT_MOUSESHADOW), {});
    }
}

Army::battle_t Battle::HeroStatus(HeroBase &hero, StatusBar &statusBar, Spell::spell_t &spell, bool quickshow, bool cansurrender, bool locked)
{
    spell = Spell::NONE;
    const ICN::icn_t sett = Settings::Get().EvilInterface() ? ICN::VGENBKGE : ICN::VGENBKG;
    const ICN::icn_t butt = ICN::VIEWGEN;
    const Surface & dialog = AGG::GetICN(sett, 0);

    Rect pos_rt;
    pos_rt.x = (display.w() - dialog.w()) / 2;
    pos_rt.y = (display.h() - dialog.h()) / 2;
    pos_rt.w = dialog.w();
    pos_rt.h = dialog.h();

    cursor.SetThemes(cursor.WAR_POINTER);
    cursor.Hide();

    Background back(pos_rt);
    back.Save();

    display.Blit(dialog, pos_rt.x, pos_rt.y);
    display.Blit(Portrait::Get(hero, Portrait::BIG), pos_rt.x + 27, pos_rt.y + 42);
    display.Blit(AGG::GetICN(butt, Color::GetIndex(hero.GetColor())+1), pos_rt.x + 148, pos_rt.y + 36);
    Point tp(pos_rt);
    std::string str;
    Text text;
    text.Set(Font::SMALL);
    str = _("%{name} the %{race}");
    String::Replace(str, "%{name}", hero.GetName());
    String::Replace(str, "%{race}", Race::String(hero.GetRace()));
    text.Set(str);
    tp.x += 8 + pos_rt.w/2 - text.w()/2;
    tp.y += 10;
    text.Blit(tp);
    str = _("Attack") + std::string(": ");
    String::AddInt(str, hero.GetAttack());
    text.Set(str);
    tp.x = pos_rt.x + 205 - text.w()/2;
    tp.y = pos_rt.y + 40;
    text.Blit(tp);
    str = _("Defense") + std::string(": ");
    String::AddInt(str, hero.GetDefense());
    text.Set(str);
    tp.x = pos_rt.x + 205 - text.w()/2;
    tp.y = pos_rt.y + 51;
    text.Blit(tp);
    str = _("Spell Power") + std::string(": ");
    String::AddInt(str, hero.GetPower());
    text.Set(str);
    tp.x = pos_rt.x + 205 - text.w()/2;
    tp.y = pos_rt.y + 62;
    text.Blit(tp);
    str = _("Knowledge") + std::string(": ");
    String::AddInt(str, hero.GetKnowledge());
    text.Set(str);
    tp.x = pos_rt.x + 205 - text.w()/2;
    tp.y = pos_rt.y + 73;
    text.Blit(tp);
    str = _("Morale") + std::string(": ") + Morale::String(hero.GetMorale());
    text.Set(str);
    tp.x = pos_rt.x + 205 - text.w()/2;
    tp.y = pos_rt.y + 84;
    text.Blit(tp);
    str = _("Luck") + std::string(": ") + Luck::String(hero.GetLuck());
    text.Set(str);
    tp.x = pos_rt.x + 205 - text.w()/2;
    tp.y = pos_rt.y + 95;
    text.Blit(tp);
    str = _("Spell Points") + std::string(": ");
    String::AddInt(str, hero.GetSpellPoints());
    str += "/";
    String::AddInt(str, hero.GetMaxSpellPoints());
    text.Set(str);
    tp.x = pos_rt.x + 205 - text.w()/2;
    tp.y = pos_rt.y + 117;
    text.Blit(tp);

    Button buttonMag(pos_rt.x + 30, pos_rt.y + 148, butt, 9, 10);
    Button buttonRet(pos_rt.x + 89, pos_rt.y + 148, butt, 11, 12);
    Button buttonSur(pos_rt.x + 148, pos_rt.y + 148, butt, 13, 14);
    Button buttonOK(pos_rt.x + 207, pos_rt.y + 148, butt, 15, 16);
    buttonMag.SetDisable(!hero.GetSpellBook().isActive() || hero.Modes(Heroes::SPELLCASTED) || locked || quickshow);
    buttonRet.SetDisable(locked || quickshow);
    buttonSur.SetDisable(!cansurrender || locked || quickshow);
    buttonMag.Draw();
    buttonRet.Draw();
    buttonSur.Draw();
    buttonOK.Draw();

    display.Flip();
    cursor.Show();

    // dialog menu loop
    while(le.HandleEvents())
    {
        if(quickshow)
        {
            if(le.MousePressRight()) continue;
            else break;
        }
            
        // exit
        if(le.KeyPress(KEY_ESCAPE)) {
            return Army::NONE;
        }
        do_button(buttonMag, {if(Spell::NONE != (spell = hero.GetSpellBook().Open(SpellBook::CMBT, true))) {back.Restore();return Army::NONE;}}, Dialog::Message(_("Cast Spell"), _("Cast a magical spell. You may only cast one spell per combat round. The round is reset when every creature has had a turn"), Font::BIG));
        do_button(buttonRet, return Army::RETREAT, Dialog::Message(_("Retreat"), _("Retreat your hero, abandoning your creatures. Your hero will be available for you to recruit again, however, the hero will have only a novice hero's forces."), Font::BIG));
        do_button(buttonSur, return Army::SURRENDER, Dialog::Message(_("Surrender"), _("Surrendering costs gold. However if you pay the ransom, the hero and all of his or her surviving creatures will be available to recruit again."), Font::BIG));
        do_button(buttonOK, return Army::NONE, Dialog::Message(_("Cancel"), _("Return to the battle."), Font::BIG));
        if(le.MouseCursor(buttonMag)) {
            statusBar.ShowMessage(_("Cast Spell"));
        } else if(le.MouseCursor(buttonRet)) {
            statusBar.ShowMessage(_("Retreat"));
        } else if(le.MouseCursor(buttonSur)) {
            statusBar.ShowMessage(_("Surrender"));
        } else if(le.MouseCursor(buttonOK)) {
            statusBar.ShowMessage(_("Cancel"));
        } else {
            statusBar.ShowMessage(_("Hero's Options"));
        }
    }
    return Army::NONE;
}
