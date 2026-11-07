#include <algorithm>
#include <android/log.h>
#include <chrono>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <pl/Mod.hpp>
#include <pl/ModMenu.hpp>
#include <pl/Input.hpp>

#include "advancements/Version.hpp"
#include "advancements/catalog.hpp"
#include "advancements/icons.hpp"
#include "advancements/java_ui.hpp"

namespace {
void logLine(std::string_view message) {
    __android_log_print(ANDROID_LOG_INFO, "Advancements", "%.*s", static_cast<int>(message.size()), message.data());
}
constexpr std::string_view kModule = "advancements_android_icons";
constexpr std::string_view kVisibleButton = "advancements.icons.open";
constexpr std::string_view kUiStyle = "ui_style";
constexpr std::string_view kPopup = "popup";
constexpr std::string_view kChat = "chat";

constexpr uint32_t kBlack = 0xFF111111u;
constexpr uint32_t kFrame = 0xFFD8D8D8u;
constexpr uint32_t kFrameLight = 0xFFF4F4F4u;
constexpr uint32_t kFrameDark = 0xFF777777u;
constexpr uint32_t kHeader = 0xFFBDBDBDu;
constexpr uint32_t kHeaderText = 0xFF262626u;
constexpr uint32_t kBoardDark = 0xFF4E171Du;
constexpr uint32_t kTab = 0xFFE3E3E3u;
constexpr uint32_t kTabDark = 0xFF707070u;
constexpr uint32_t kObtained = 0xFFE1AF17u;
constexpr uint32_t kObtainedDark = 0xFF8F6500u;
constexpr uint32_t kLocked = 0xFF737373u;
constexpr uint32_t kText = 0xFFF5F5F5u;
constexpr uint32_t kLockedText = 0xFF9B9B9Bu;

bool gOpen = false;
float gUiScale = 1.0f;
bool gJavaStyle = false;
bool gPopupEnabled = true;
bool gChatEnabled = true;
std::string gPopupTitle;
std::string gPopupDescription;
std::uint64_t gPopupUntilMs = 0;
std::string gPopupId;
void drawPanel();
void drawPopupOnly();
void syncButtonVisibility();
int gCategory = 0;
std::size_t gSelected = 0;
std::vector<bool> gCompleted;
std::uintptr_t gWorldKey = 0;
std::unordered_map<std::uintptr_t, std::vector<bool>> gWorldCompleted;
std::vector<unsigned char> gBoardTexture;
struct SurfaceSize { float width{}; float height{}; };
SurfaceSize gHudSurface{1920.f, 1080.f};

float gPanelX = 0.f, gPanelY = 0.f, gPanelW = 0.f, gPanelH = 0.f;
float gTabX = 0.f, gTabW = 0.f, gTabY = 0.f, gTabH = 0.f;
float gViewX = 0.f, gViewY = 0.f, gViewW = 0.f, gViewH = 0.f;
float gNode = 0.f, gStepX = 0.f, gStepY = 0.f;

char entryGlyph(const char *title);

const advancements::icons::Icon &iconById(std::string_view id) {
    for (const auto &icon : advancements::icons::all) if (icon.id == id) return icon;
    return advancements::icons::all[0];
}

const advancements::icons::Icon &iconFor(const char *id, const char *title, const char *description, int category) {
    const std::string idText = id ? id : "";
    std::string text = title ? title : "";
    text += " ";
    if (description) text += description;
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    auto has = [&](std::string_view value) { return text.find(value) != std::string::npos; };
    if (idText.find("kill_") != std::string::npos || has("monster") || has("mob") || has("warden")) return iconById("diamond_sword");
    if (idText.find("shoot") != std::string::npos || idText.find("sniper") != std::string::npos || has("bow") || has("arrow")) return iconById("bow");
    if (idText.find("fish") != std::string::npos || idText.find("boat") != std::string::npos || has("fishing") || has("rod")) return iconById("fishing_rod");
    if (idText.find("elytra") != std::string::npos || idText.find("levitate") != std::string::npos || has("elytra") || has("fly")) return iconById("elytra");
    if (idText.find("dragon") != std::string::npos || idText.find("end/") != std::string::npos || has("dragon") || has("ender") || has("eye")) return iconById("elytra");
    if (idText.find("nether") != std::string::npos || idText.find("wither") != std::string::npos || has("netherite") || has("ancient debris") || has("wither")) return iconById("netherite_sword");
    if (idText.find("diamond") != std::string::npos || has("diamond")) return iconById("diamond");
    if (idText.find("fish") != std::string::npos || has("fish")) return iconById("fishing_rod");
    if (idText.find("food") != std::string::npos || idText.find("breed") != std::string::npos || has("apple") || has("food") || has("eat") || has("animal") || has("honey") || has("bee")) return iconById("apple");
    if (idText.find("bread") != std::string::npos || has("bread") || has("cake")) return iconById("bread");
    if (idText.find("potion") != std::string::npos || has("potion") || has("effect")) return iconById("apple");
    if (idText.find("craft") != std::string::npos || idText.find("mine") != std::string::npos || idText.find("tool") != std::string::npos || has("craft") || has("stone") || has("mine") || has("tool") || has("iron")) return iconById("diamond_pickaxe");
    if (category == 1) return iconById("netherite_sword");
    if (category == 2) return iconById("elytra");
    if (category == 4) return iconById("apple");
    return iconById("crafting_table");
}

std::string iconImageId(const advancements::icons::Icon &icon) {
    return std::string("advancement.icon.") + std::string(icon.id);
}

std::size_t categoryBase(int category) {
    std::size_t base = 0;
    for (int i = 0; i < category; ++i) base += advancements::counts[i];
    return base;
}

bool completed(int category, std::size_t index) {
    const auto absolute = categoryBase(category) + index;
    return absolute < gCompleted.size() && gCompleted[absolute];
}

std::uint64_t nowMs() {
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
}

void announceCompletion(int category, std::size_t index) {
    const auto &entry = advancements::entries[category][index];
    logLine("completion detected: category="+std::to_string(category)+" index="+std::to_string(index)+" id="+entry.id+" title="+entry.title);
    gCompleted[categoryBase(category) + index] = true;
    if (gWorldKey != 0) gWorldCompleted[gWorldKey] = gCompleted;
    if (gChatEnabled) {
        logLine(std::string("advancement chat notification requested: ") + entry.title);
    }
    if (gPopupEnabled) {
        gPopupId = entry.id;
        gPopupTitle = entry.title;
        gPopupDescription = entry.description;
        gPopupUntilMs = nowMs() + 5000;
        logLine("advancement popup queued: id="+gPopupId+" title="+gPopupTitle+" durationMs=5000");
        if (!gOpen) drawPopupOnly();
    }
    if (gOpen) drawPanel();
}

void markCompletedByText(std::string_view text) {
    if (text.empty()) return;
    std::string folded(text);
    std::transform(folded.begin(), folded.end(), folded.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    for (int category = 0; category < 5; ++category) {
        for (std::size_t i = 0; i < advancements::counts[category]; ++i) {
            const auto &entry = advancements::entries[category][i];
            const auto title = std::string_view(entry.title);
            std::string foldedTitle(title);
            std::transform(foldedTitle.begin(), foldedTitle.end(), foldedTitle.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            bool matches = folded.find(foldedTitle) != std::string::npos;
            // Stone Age is a Java advancement. Bedrock does not emit a toast
            // with that title; depending on the localization/build it emits
            // the Bedrock achievement key or the related "Getting an
            // Upgrade" title instead. Keep the Java catalog entry while
            // accepting those real Bedrock identifiers as aliases.
            if (category == 0 && i == 10) {
                matches = matches || folded.find("achievement.getstone") != std::string::npos
                    || folded.find("get stone") != std::string::npos
                    || folded.find("mine stone") != std::string::npos
                    || folded.find("getting an upgrade") != std::string::npos
                    || folded.find("stone_pickaxe") != std::string::npos;
            }
            if (matches && !gCompleted[categoryBase(category) + i]) announceCompletion(category, i);
        }
    }
}

void createBoardTexture() {
    constexpr int w = 64, h = 64;
    gBoardTexture.resize(w * h * 4);
    for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
        const int cell = ((x / 4) * 17 + (y / 4) * 31 + (x * y) / 7) & 31;
        const bool seam = (x % 8 == 0) || (y % 9 == 0);
        const auto r = static_cast<unsigned char>(seam ? 72 + cell : 104 + cell);
        const auto g = static_cast<unsigned char>(seam ? 20 + cell / 3 : 30 + cell / 2);
        const auto b = static_cast<unsigned char>(seam ? 25 + cell / 3 : 40 + cell / 2);
        const auto p = static_cast<std::size_t>((y * w + x) * 4);
        gBoardTexture[p] = r; gBoardTexture[p + 1] = g; gBoardTexture[p + 2] = b; gBoardTexture[p + 3] = 255;
    }
}

void drawDefaultPanel() {
    if (!gOpen) { pl::modmenu::submitDrawCommands(kModule, {}); return; }
    if (gUiScale <= 0.001f) { pl::modmenu::submitDrawCommands(kModule, {}); return; }
    const auto surface = gHudSurface;
    if (surface.width <= 0.f || surface.height <= 0.f) return;
    const float scale = std::clamp(std::min(surface.width / 1120.f, surface.height / 560.f), 0.72f, 1.35f) * std::clamp(gUiScale, 0.0f, 1.0f);
    const float w = 840.f * scale, h = 500.f * scale;
    const float x = (surface.width - w) * .5f, y = (surface.height - h) * .5f;
    gPanelX = x; gPanelY = y; gPanelW = w; gPanelH = h;
    const float border = 5.f * scale, headerH = 48.f * scale;
    const float tabH = 62.f * scale, tabGap = 5.f * scale, tabW = 126.f * scale;
    const float tabY = y - 44.f * scale;
    gTabX = x + 64.f * scale; gTabY = tabY; gTabW = tabW; gTabH = tabH;

    std::vector<pl::modmenu::DrawCommand> d;
    d.reserve(400);
    auto rect = [&](float rx, float ry, float rw, float rh, uint32_t c) {
        pl::modmenu::DrawCommand q{}; q.type = pl::modmenu::DrawCommandType::RectFilled; q.x=rx; q.y=ry; q.w=rw; q.h=rh; q.color=c; d.push_back(std::move(q));
    };
    auto outline = [&](float rx, float ry, float rw, float rh, uint32_t c) {
        pl::modmenu::DrawCommand q{}; q.type = pl::modmenu::DrawCommandType::Rect; q.x=rx; q.y=ry; q.w=rw; q.h=rh; q.color=c; d.push_back(std::move(q));
    };
    auto text = [&](float tx, float ty, float tw, float th, uint32_t c, float size, std::string_view value) {
        pl::modmenu::DrawCommand q{}; q.type=pl::modmenu::DrawCommandType::Text; q.x=tx; q.y=ty; q.w=tw; q.h=th; q.color=c; q.size=size; q.text=value; q.fontId="Minecraft"; d.push_back(std::move(q));
    };
    auto image = [&](float ix, float iy, float iw, float ih, std::string_view id) {
        pl::modmenu::DrawCommand q{}; q.type=pl::modmenu::DrawCommandType::Image; q.x=ix; q.y=iy; q.w=iw; q.h=ih; q.imageId=id; d.push_back(std::move(q));
    };
    auto pixelIcon = [&](float ix, float iy, float iw, float ih, const advancements::icons::Icon &icon, bool dimmed) {
        const auto *px = icon.rgba.data();
        const float cellW = iw / 16.f, cellH = ih / 16.f;
        for (int py=0; py<16; ++py) for (int pxel=0; pxel<16; ++pxel) {
            const int sx = pxel * 2, sy = py * 2;
            const std::size_t p = static_cast<std::size_t>((sy * icon.width + sx) * 4);
            const unsigned char alpha = px[p + 3];
            if (alpha < 24) continue;
            unsigned char r = px[p], g = px[p+1], b = px[p+2];
            if (dimmed) { r = static_cast<unsigned char>(r * 0.42f); g = static_cast<unsigned char>(g * 0.42f); b = static_cast<unsigned char>(b * 0.42f); }
            pl::modmenu::DrawCommand q{}; q.type=pl::modmenu::DrawCommandType::RectFilled;
            q.x=ix+pxel*cellW; q.y=iy+py*cellH; q.w=cellW+0.5f; q.h=cellH+0.5f;
            q.color=(static_cast<uint32_t>(alpha)<<24)|(static_cast<uint32_t>(r)<<16)|(static_cast<uint32_t>(g)<<8)|b;
            d.push_back(std::move(q));
        }
    };
    auto line = [&](float ax, float ay, float bx, float by, uint32_t c, float width) {
        pl::modmenu::DrawCommand q{}; q.type=pl::modmenu::DrawCommandType::Line; q.x=ax; q.y=ay; q.x3=bx; q.y3=by; q.color=c; q.size=width; d.push_back(std::move(q));
    };

    rect(x+8*scale,y+9*scale,w,h,kBlack); rect(x,y,w,h,kFrameDark);
    rect(x+border,y+border,w-2*border,h-2*border,kFrame);
    rect(x+border,y+border,w-2*border,headerH,kHeader);
    outline(x,y,w,h,kBlack); outline(x+border,y+border,w-2*border,h-2*border,kFrameLight);
    text(x+18*scale,y+14*scale,w-90*scale,23*scale,kHeaderText,20*scale,"Advancements");
    text(x+w-35*scale,y+13*scale,25*scale,24*scale,kFrameLight,20*scale,"×");

    static constexpr const char *glyphs[] = {"M", "N", "E", "A", "H"};
    for (int i=0;i<5;++i) {
        const float tx=gTabX+i*(tabW+tabGap); const bool selected=i==gCategory;
        rect(tx+3*scale,tabY+5*scale,tabW,tabH,kBlack);
        rect(tx,tabY,tabW,tabH,selected?kObtained:kTab);
        outline(tx,tabY,tabW,tabH,selected?kObtainedDark:kTabDark);
        rect(tx+10*scale,tabY+8*scale,42*scale,42*scale,selected?kObtainedDark:kFrameDark);
        text(tx+10*scale,tabY+15*scale,42*scale,28*scale,kBlack,22*scale,glyphs[i]);
        text(tx+59*scale,tabY+18*scale,tabW-64*scale,25*scale,selected?kBlack:kHeaderText,11*scale,advancements::names[i]);
    }

    const float viewX=x+20*scale, viewY=y+headerH+18*scale, viewW=w-40*scale, viewH=h-headerH-34*scale;
    gViewX=viewX;gViewY=viewY;gViewW=viewW;gViewH=viewH;
    rect(viewX,viewY,viewW,viewH,kBoardDark); image(viewX,viewY,viewW,viewH,"advancements.board"); outline(viewX,viewY,viewW,viewH,kBlack);

    const auto *entries=advancements::entries[gCategory]; const std::size_t count=advancements::counts[gCategory];
    if (count && gSelected>=count) gSelected=0;
    const int columns=8; const float node=34*scale, startX=viewX+34*scale, startY=viewY+34*scale, stepX=78*scale, stepY=54*scale;
    gNode=node;gStepX=stepX;gStepY=stepY;
    for (std::size_t i=1;i<count;++i) {
        const int col=static_cast<int>((i-1)%columns), row=static_cast<int>((i-1)/columns);
        const float ax=startX+col*stepX+node*.5f, ay=startY+row*stepY+node*.5f;
        const float bx=startX+(col+1)%columns*stepX+node*.5f, by=startY+row*stepY+node*.5f;
        line(ax,ay,bx,by,completed(gCategory,i)?kObtained:kFrameDark,3*scale);
    }
    for (std::size_t i=0;i<count;++i) {
        const int col=static_cast<int>(i%columns), row=static_cast<int>(i/columns);
        const float nx=startX+col*stepX, ny=startY+row*stepY; const bool done=completed(gCategory,i), selected=i==gSelected;
        rect(nx+4*scale,ny+5*scale,node,node,kBlack);
        rect(nx,ny,node,node,done?kObtained:(selected?kFrameLight:kLocked));
        outline(nx,ny,node,node,selected?kBlack:(done?kObtainedDark:kTabDark));
        rect(nx+8*scale,ny+8*scale,node-16*scale,node-16*scale,done?kObtainedDark:kTabDark);
        const auto &icon = iconFor(entries[i].id, entries[i].title, entries[i].description, gCategory);
        pixelIcon(nx+9*scale, ny+9*scale, node-18*scale, node-18*scale, icon, !done);
    }
    if (!count) text(viewX+25*scale,viewY+viewH*.5f,viewW-50*scale,20*scale,kText,15*scale,"Nothing to display");

    if (count) {
        const auto &entry=entries[gSelected]; const bool done=completed(gCategory,gSelected);
        const float infoW=std::min(390*scale,viewW-24*scale), infoH=92*scale;
        const float infoX=viewX+viewW-infoW-10*scale, infoY=viewY+viewH-infoH-10*scale;
        rect(infoX+4*scale,infoY+4*scale,infoW,infoH,kBlack); rect(infoX,infoY,infoW,infoH,0xE8202020u);
        outline(infoX,infoY,infoW,infoH,done?kObtained:kTabDark);
        const auto &selectedIcon = iconFor(entry.id, entry.title, entry.description, gCategory);
        pixelIcon(infoX+infoW-48*scale, infoY+8*scale, 34*scale, 34*scale, selectedIcon, !done);
        text(infoX+12*scale,infoY+8*scale,infoW-24*scale,18*scale,done?kObtained:kText,14*scale,entry.title);
        text(infoX+12*scale,infoY+31*scale,infoW-24*scale,30*scale,done?kText:kLockedText,10*scale,entry.description);
        text(infoX+12*scale,infoY+69*scale,infoW-24*scale,14*scale,done?kObtained:kLockedText,10*scale,done?"Completed":"Locked");
    }
    pl::modmenu::submitDrawCommands(kModule,d);
}

std::string javaTextureId(std::string_view id) {
    return std::string("advancements.java.") + std::string(id);
}

void drawJavaPanel() {
    if (!gOpen) { pl::modmenu::submitDrawCommands(kModule, {}); return; }
    if (gUiScale <= 0.001f) { pl::modmenu::submitDrawCommands(kModule, {}); return; }
    const auto surface = gHudSurface;
    if (surface.width <= 0.f || surface.height <= 0.f) return;
    const float scale = std::clamp(std::min(surface.width / 720.f, surface.height / 470.f), 0.70f, 1.45f) * gUiScale;
    const float w=720.f*scale,h=470.f*scale,x=(surface.width-w)*.5f,y=(surface.height-h)*.5f;
    gPanelX=x; gPanelY=y; gPanelW=w; gPanelH=h;
    gTabX=x+14.f*scale; gTabY=y+8.f*scale; gTabW=92.f*scale; gTabH=58.f*scale;
    gViewX=x+14.f*scale; gViewY=y+78.f*scale; gViewW=w-28.f*scale; gViewH=h-92.f*scale;
    gNode=42.f*scale; gStepX=82.f*scale; gStepY=66.f*scale;

    std::vector<pl::modmenu::DrawCommand> d;
    d.reserve(700);
    auto rect=[&](float a,float b,float c,float e,std::uint32_t col){pl::modmenu::DrawCommand q{};q.type=pl::modmenu::DrawCommandType::RectFilled;q.x=a;q.y=b;q.w=c;q.h=e;q.color=col;d.push_back(std::move(q));};
    auto outline=[&](float a,float b,float c,float e,std::uint32_t col){pl::modmenu::DrawCommand q{};q.type=pl::modmenu::DrawCommandType::Rect;q.x=a;q.y=b;q.w=c;q.h=e;q.color=col;d.push_back(std::move(q));};
    auto text=[&](float a,float b,float c,float e,std::uint32_t col,float size,std::string_view value){pl::modmenu::DrawCommand q{};q.type=pl::modmenu::DrawCommandType::Text;q.x=a;q.y=b;q.w=c;q.h=e;q.color=col;q.size=size;q.text=value;q.fontId="Minecraft";d.push_back(std::move(q));};
    rect(x+7.f*scale,y+8.f*scale,w,h,0xFF101010u); rect(x,y,w,h,0xFFD0D0D0u); outline(x,y,w,h,0xFF202020u);
    rect(x+5.f*scale,y+5.f*scale,w-10.f*scale,h-10.f*scale,0xFFB9B9B9u);
    text(x+19.f*scale,y+16.f*scale,w-40.f*scale,28.f*scale,0xFF202020u,22.f*scale,"Advancements");
    const char *tabNames[] = {"Minecraft","Nether","The End","Adventure","Husbandry"};
    const char *tabGlyphs[] = {"M","N","E","A","H"};
    for (int i=0;i<5;++i) {
        const float tx=gTabX+i*(gTabW+6.f*scale); const bool selected=i==gCategory;
        rect(tx+4.f*scale,gTabY+5.f*scale,gTabW,gTabH,0xFF171717u);
        rect(tx,gTabY,gTabW,gTabH,selected?0xFFD39A00u:0xFF777777u);
        outline(tx,gTabY,gTabW,gTabH,0xFF202020u);
        text(tx+10.f*scale,gTabY+15.f*scale,30.f*scale,28.f*scale,0xFF101010u,22.f*scale,tabGlyphs[i]);
        text(tx+42.f*scale,gTabY+19.f*scale,gTabW-46.f*scale,20.f*scale,0xFF101010u,10.f*scale,tabNames[i]);
    }
    rect(gViewX,gViewY,gViewW,gViewH,0xFF353535u); outline(gViewX,gViewY,gViewW,gViewH,0xFF151515u);
    const auto *entries=advancements::entries[gCategory]; const auto count=advancements::counts[gCategory];
    const float startX=gViewX+28.f*scale, startY=gViewY+28.f*scale;
    for (std::size_t i=1;i<count;++i) {
        const int col=static_cast<int>((i-1)%7), row=static_cast<int>((i-1)/7);
        const float ax=startX+col*gStepX+gNode*.5f, ay=startY+row*gStepY+gNode*.5f;
        const float bx=startX+((i)%7)*gStepX+gNode*.5f;
        pl::modmenu::DrawCommand line{}; line.type=pl::modmenu::DrawCommandType::Line; line.x=ax; line.y=ay; line.x3=bx; line.y3=ay; line.color=completed(gCategory,i)?0xFFD39A00u:0xFF777777u; line.size=3.f*scale; d.push_back(std::move(line));
    }
    for (std::size_t i=0;i<count;++i) {
        const int col=static_cast<int>(i%7), row=static_cast<int>(i/7); const float nx=startX+col*gStepX, ny=startY+row*gStepY;
        const bool done=completed(gCategory,i), selected=i==gSelected;
        rect(nx+4.f*scale,ny+5.f*scale,gNode,gNode,0xFF111111u); rect(nx,ny,gNode,gNode,done?0xFFD39A00u:(selected?0xFFD0D0D0u:0xFF6B6B6Bu)); outline(nx,ny,gNode,gNode,selected?0xFFFFFFFFu:0xFF202020u);
        const auto &ic=iconFor(entries[i].id,entries[i].title,entries[i].description,gCategory);
        for (int py=0;py<16;++py) for (int px=0;px<16;++px) { const auto p=static_cast<std::size_t>(((py*2)*ic.width+(px*2))*4); if(ic.rgba[p+3]<24) continue; auto rr=ic.rgba[p],gg=ic.rgba[p+1],bb=ic.rgba[p+2]; if(!done){rr=static_cast<unsigned char>(rr*.4f);gg=static_cast<unsigned char>(gg*.4f);bb=static_cast<unsigned char>(bb*.4f);} rect(nx+5.f*scale+px*(gNode-10.f*scale)/16.f,ny+5.f*scale+py*(gNode-10.f*scale)/16.f,(gNode-10.f*scale)/16.f+0.5f,(gNode-10.f*scale)/16.f+0.5f,(0xFFu<<24)|(rr<<16)|(gg<<8)|bb); }
    }
    if (count) {
        const auto &entry=entries[gSelected]; const bool done=completed(gCategory,gSelected); const float infoW=330.f*scale, infoH=80.f*scale, infoX=gViewX+gViewW-infoW-10.f*scale, infoY=gViewY+gViewH-infoH-10.f*scale;
        rect(infoX+4.f*scale,infoY+4.f*scale,infoW,infoH,0xFF111111u); rect(infoX,infoY,infoW,infoH,0xEE202020u); outline(infoX,infoY,infoW,infoH,done?0xFFD39A00u:0xFF777777u); text(infoX+10.f*scale,infoY+8.f*scale,infoW-20.f*scale,20.f*scale,done?0xFFFFD83Du:0xFFFFFFFFu,14.f*scale,entry.title); text(infoX+10.f*scale,infoY+31.f*scale,infoW-20.f*scale,28.f*scale,0xFFE0E0E0u,10.f*scale,entry.description); text(infoX+10.f*scale,infoY+61.f*scale,infoW-20.f*scale,14.f*scale,done?0xFFFFD83Du:0xFFAAAAAAu,10.f*scale,done?"Completed":"Locked");
    }
    pl::modmenu::submitDrawCommands(kModule,d);
}
void drawPanel() { if (gJavaStyle) drawJavaPanel(); else drawDefaultPanel(); }

void drawPopupOnly() {
    if (!gPopupEnabled || gPopupUntilMs <= nowMs()) { pl::modmenu::submitDrawCommands(kModule, {}); return; }
    const auto surface = gHudSurface;
    const float scale = std::clamp(std::min(surface.width / 1000.f, surface.height / 620.f), 0.65f, 1.15f) * gUiScale;
    // Bedrock places recipe-unlocked notifications in the upper-right area.
    // Keep this mod's advancement toast in the lower-right so both remain
    // visible when they appear at the same time.
    const float w=360*scale,h=72*scale,x=surface.width-w-22*scale,y=surface.height-h-22*scale;
    std::vector<pl::modmenu::DrawCommand> d; d.reserve(32);
    auto rect=[&](float a,float b,float c,float e,uint32_t col){pl::modmenu::DrawCommand q{};q.type=pl::modmenu::DrawCommandType::RectFilled;q.x=a;q.y=b;q.w=c;q.h=e;q.color=col;d.push_back(std::move(q));};
    auto outline=[&](float a,float b,float c,float e,uint32_t col){pl::modmenu::DrawCommand q{};q.type=pl::modmenu::DrawCommandType::Rect;q.x=a;q.y=b;q.w=c;q.h=e;q.color=col;d.push_back(std::move(q));};
    auto text=[&](float a,float b,float c,float e,uint32_t col,float size,std::string_view value){pl::modmenu::DrawCommand q{};q.type=pl::modmenu::DrawCommandType::Text;q.x=a;q.y=b;q.w=c;q.h=e;q.color=col;q.size=size;q.text=value;q.fontId="Minecraft";d.push_back(std::move(q));};
    auto image=[&](float a,float b,float c,float e,std::string_view id){pl::modmenu::DrawCommand q{};q.type=pl::modmenu::DrawCommandType::Image;q.x=a;q.y=b;q.w=c;q.h=e;q.imageId=id;d.push_back(std::move(q));};
    if (gJavaStyle) image(x,y,w,h,javaTextureId("box_obtained")); else { rect(x+4*scale,y+4*scale,w,h,0xFF111111u); rect(x,y,w,h,0xEE222222u); outline(x,y,w,h,0xFFFFC928u); }
    image(x+10*scale,y+10*scale,50*scale,50*scale,iconImageId(iconFor(gPopupId.c_str(),gPopupTitle.c_str(),gPopupDescription.c_str(),gCategory)));
    text(x+70*scale,y+10*scale,w-82*scale,18*scale,0xFFFFD83Du,13*scale,gPopupTitle);
    text(x+70*scale,y+32*scale,w-82*scale,28*scale,0xFFFFFFFFu,9*scale,gPopupDescription);
    pl::modmenu::submitDrawCommands(kModule,d);
}

char entryGlyph(const char *title) { return title && *title ? static_cast<char>(std::toupper(static_cast<unsigned char>(*title))) : '?'; }

bool onTouch(const pl::input::TouchEvent &event) {
    if (!gOpen || event.action != 1) return false;
    if (event.x < gPanelX || event.x > gPanelX+gPanelW || event.y < gTabY || event.y > gPanelY+gPanelH) return false;
    const float gap=5*(gPanelW/840.f);
    if (event.y>=gTabY && event.y<=gTabY+gTabH) {
        for (int i=0;i<5;++i) { const float left=gTabX+i*(gTabW+gap); if(event.x>=left&&event.x<=left+gTabW){gCategory=i;gSelected=0;drawPanel();return true;} }
    }
    if (event.x>=gViewX&&event.x<=gViewX+gViewW&&event.y>=gViewY&&event.y<=gViewY+gViewH) {
        const auto count=advancements::counts[gCategory]; const float startX=gViewX+(gJavaStyle?28.f*(gPanelW/720.f):34.f*(gPanelW/840.f)), startY=gViewY+(gJavaStyle?28.f*(gPanelW/720.f):34.f*(gPanelW/840.f));
        const int columns=gJavaStyle?7:8; const int col=static_cast<int>((event.x-startX)/gStepX), row=static_cast<int>((event.y-startY)/gStepY);
        if(col>=0&&col<columns&&row>=0){const auto index=static_cast<std::size_t>(row*columns+col);const float nx=startX+col*gStepX,ny=startY+row*gStepY;if(index<count&&event.x>=nx&&event.x<=nx+gNode&&event.y>=ny&&event.y<=ny+gNode){gSelected=index;drawPanel();return true;}}
    }
    return true;
}

void onConfigChanged(std::string_view moduleId, std::string_view key, std::string_view value) {
    if (moduleId != kModule) return;
    logLine("config changed: key="+std::string(key)+" value="+std::string(value));
    if (key==kUiStyle) {
        std::string style(value);
        std::transform(style.begin(), style.end(), style.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        gJavaStyle = style == "1" || style == "java" || style == "java ui" ||
                     style == "revamped" || style == "revamped ui" ||
                     style.find("java") != std::string::npos ||
                     style.find("revamped") != std::string::npos;
        logLine(std::string("Revamped UI state: ") + (gJavaStyle ? "enabled" : "disabled"));
        drawPanel();
    }
    if (key==kPopup) { gPopupEnabled=value=="1"; if (!gPopupEnabled) drawPanel(); }
    if (key==kChat) { gChatEnabled=value=="1"; }
    if (key=="ui_scale" || key=="scale") {
        const std::string safeValue(value);
        char *end = nullptr;
        const float parsed = std::strtof(safeValue.c_str(), &end);
        if (end != safeValue.c_str()) gUiScale = std::clamp(parsed, 0.0f, 1.0f);
        drawPanel();
    }
}

void onVisibleButton(std::string_view, pl::modmenu::ButtonEvent event, float) {
    if (event != pl::modmenu::ButtonEvent::Click) return;
    gOpen = !gOpen;
    if (gOpen) gSelected = 0;
    drawPanel();
}

bool registerVisibleButton() {
    pl::modmenu::ButtonInfo info;
    info.buttonId = std::string(kVisibleButton);
    info.moduleId = std::string(kModule);
    info.displayName = "Advancements";
    info.label = "Advancements";
    info.behavior = pl::modmenu::ButtonBehavior::Click;
    info.defaultVisible = true;
    info.stylePreset = pl::modmenu::ButtonStylePreset::Accent;
    info.normalBgColor = 0xCC333333u;
    info.activeBgColor = 0xCC6B42C0u;
    info.borderColor = 0xFFE0C4FFu;
    info.textColor = 0xFFFFFFFFu;
    info.activeTextColor = 0xFFFFFFFFu;
    info.widthScale = 1.25f;
    info.heightScale = 1.0f;
    info.hideLabelWhenIconPresent = false;
    info.onEvent = onVisibleButton;
    return pl::modmenu::registerButton(info);
}

void syncButtonVisibility() {
    pl::modmenu::unregisterButton(kVisibleButton);
    registerVisibleButton();
    pl::modmenu::setModuleEnabled(kModule, true);
}

class AdvancementsClean {
public:
    bool load(pl::mod::ModContext &) {
        logLine("load begin: version="+std::string(advancements::Version));
        createBoardTexture();
        if(!pl::modmenu::ModuleBuilder(std::string(kModule),"Advancements")
            .description("Java-style advancement trees with icons, labels, descriptions, and live achievement updates.")
            .defaultEnabled(true).hideInHudEditor(true)
            .config("ui_scale","Advancements UI scale",pl::modmenu::ConfigType::SliderFloat,"1.00","0.00","1.00")
            .config("ui_style","Revamped UI",pl::modmenu::ConfigType::Radio,"0","Default UI,Revamped UI")
            .config("popup","Advancement popup",pl::modmenu::ConfigType::Toggle,"1")
            .config("chat","Advancement chat message",pl::modmenu::ConfigType::Toggle,"1")
            .onConfigChanged(onConfigChanged).registerModule()) { logLine("load failed: module registration"); return false; }
        const bool buttonRegistered = registerVisibleButton();
        logLine(std::string("initial floating fallback registration: ") +
                (buttonRegistered ? "success" : "failed"));
        gCompleted.assign(256,false);
        pl::input::registerTouchCallback(onTouch);
        logLine("load complete: standalone Preloader UI and touch listeners registered");
        return true;
    }
    bool enable(pl::mod::ModContext &) { logLine("enable"); pl::modmenu::setModuleEnabled(kModule,true); syncButtonVisibility(); return true; }
    bool disable(pl::mod::ModContext &) { logLine("disable"); gOpen=false;drawPanel();pl::modmenu::setModuleEnabled(kModule,false);pl::modmenu::unregisterButton(kVisibleButton); return true; }
    bool unload(pl::mod::ModContext &) {
        logLine("unload"); gOpen=false;pl::modmenu::submitDrawCommands(kModule,{});
        pl::modmenu::unregisterButton(kVisibleButton);
        gWorldKey=0; gWorldCompleted.clear();
        pl::modmenu::unregisterModule(kModule);gBoardTexture.clear();gCompleted.clear();return true;
    }
};
}

PL_REGISTER_MOD(AdvancementsClean, AdvancementsClean{})
