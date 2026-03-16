#include <asm/bios.h>
#include <libtoncar/vram.h>
#include <oam.h>
#include <palram.h>
#include <registers/display.h>
#include <sprite.h>
#include <vram.h>

#include <cstdint>
#include <cstdlib>

#include "example/assets/gfx/example.h"
#include "interrupt_manager.h"
#include "libtoncar/colors.h"
#include "libtoncar/oam.h"
#include "libtoncar/palram.h"
#include "libtoncar/registers/display.h"
#include "libtoncar/screen.h"
#include "mgba/logger.h"

// NOLINTNEXTLINE(google-build-using-namespace)
using namespace toncar;

namespace {
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
uint32_t vblank_counter{0};

void VBlankCounter() {
  ++vblank_counter;
  MGBA_LOG_DEBUG("VBlank counter: %lu", static_cast<unsigned long>(vblank_counter));
}
}  // namespace

int main() {
  InterruptManager::Instance()
      .SetInterruptHandler(Interrupt::VBlank, VBlankCounter)
      .SetInterruptHandler(Interrupt::VBlank, VBlankCounter)
      .EnableInterrupt(Interrupt::VBlank);

  Dispcnt& dispcnt = Dispcnt::Instance();

  MGBA_LOG_INFO("Start");

  dispcnt.Reset().SetMode(Dispcnt::Mode::DcntMode3).SetLayer<Dispcnt::Layer::DcntBg2>();

  Screen& screen = Screen::Instance();

  screen.Mode3WritePixel(120, 80, colors15::kRed)
      .Mode3WritePixel(136, 80, colors15::kGreen)
      .Mode3WritePixel(120, 96, colors15::kBlue);

  dispcnt.Reset()
      .SetMode(Dispcnt::Mode::DcntMode0)
      .SetLayer<Dispcnt::Layer::DcntBg2>()
      .SetLayer<Dispcnt::Layer::DcntObj>()
      .Set1dMapping();

  Palram::Instance().SpritesPalbank().LoadPalette(sprites::example::french.GetSprite(0).Palette());
  Vram::Instance().SpriteCharblock(0).LoadTiles(sprites::example::french.GetSprite(0));
  Vram::Instance().SpriteCharblock(0).LoadTiles(sprites::example::germany.GetSprite(0));
  Vram::Instance().SpriteCharblock(0).LoadTiles(sprites::example::smiley.GetSprite(0));

  Oam::ObjAttr& france_obj{Oam::Instance().RefSpriteAttributes(0)};
  france_obj.attr0.SetY(50)
      .SetOm(Oam::ObjAttr0::OM::NormalRendering)
      .SetGm(Oam::ObjAttr0::GM::NormalRendering)
      .SetShape(Oam::ObjAttr0::Shape::Square)
      .SetColorMode4bpp();
  france_obj.attr1.SetX(30).SetSize(Oam::ObjAttr1::Size::Medium);
  france_obj.attr2.Set4bppPalbank(0).SetPrio(1).SetTileId(0);

  Oam::ObjAttr& germany_obj{Oam::Instance().RefSpriteAttributes(1)};
  germany_obj.attr0.SetY(30)
      .SetOm(Oam::ObjAttr0::OM::NormalRendering)
      .SetGm(Oam::ObjAttr0::GM::NormalRendering)
      .SetShape(Oam::ObjAttr0::Shape::Square)
      .SetColorMode4bpp();
  germany_obj.attr1.SetX(180).SetSize(Oam::ObjAttr1::Size::Medium);
  germany_obj.attr2.Set4bppPalbank(0).SetPrio(1).SetTileId(16);

  Oam::ObjAttr& smiley_obj{Oam::Instance().RefSpriteAttributes(2)};
  smiley_obj.attr0.SetY(10)
      .SetOm(Oam::ObjAttr0::OM::NormalRendering)
      .SetGm(Oam::ObjAttr0::GM::NormalRendering)
      .SetShape(Oam::ObjAttr0::Shape::Square)
      .SetColorMode4bpp();
  smiley_obj.attr1.SetX(150).SetSize(Oam::ObjAttr1::Size::Medium);
  smiley_obj.attr2.Set4bppPalbank(0).SetPrio(1).SetTileId(32);

  while (true) {
    VBlankIntrWait();
    if (vblank_counter % 2 == 0) {
      france_obj.attr1.SetX(france_obj.attr1.GetX() + 1);
    }
    if (vblank_counter % 10 == 0) {
      smiley_obj.attr0.SetY(smiley_obj.attr0.GetY() + 1);
    }
    if (vblank_counter % 20 == 0) {
      germany_obj.attr1.ToggleVerticalFlip();
    }
  }
}
