use image::{DynamicImage, GenericImageView, Pixel};

use crate::{
    color::{Color, Palette16},
    error::GfxConverterError,
};

const TILE_WIDTH: u8 = 8;

fn valid_sprite_size(width: u32, height: u32) -> bool {
    matches!(
        (width, height),
        (8, 8)
            | (16, 16)
            | (32, 32)
            | (64, 64)
            | (16, 8)
            | (32, 8)
            | (32, 16)
            | (64, 32)
            | (8, 16)
            | (8, 32)
            | (16, 32)
            | (32, 64)
    )
}

pub struct Sprite {
    pub data: Vec<Color>,
    pub size: (u8, u8),
}

impl Sprite {
    pub fn new(image: DynamicImage) -> Result<Self, GfxConverterError> {
        if !valid_sprite_size(image.width(), image.height()) {
            return Err(GfxConverterError::InvalidImageSize);
        }

        let pixels = (0..image.height())
            .flat_map(|y| (0..image.width()).map(move |x| (x, y)))
            .map(|(x, y)| {
                let rgba = image.get_pixel(x, y).to_rgba();
                Color::from_rgb(rgba[0], rgba[1], rgba[2], rgba[3])
            })
            .collect();

        Ok(Sprite {
            data: pixels,
            size: (image.width() as u8, image.height() as u8),
        })
    }
}

pub struct TiledSprite {
    data: Vec<[u8; (TILE_WIDTH * TILE_WIDTH) as usize]>,
    pub size: (u8, u8),
}

impl TiledSprite {
    pub fn new(sprite: Sprite, palette: &mut Palette16) -> Result<Self, GfxConverterError> {
        let n_tiles_x = sprite.size.0 / TILE_WIDTH;
        let n_tiles_y = sprite.size.1 / TILE_WIDTH;

        let mut tiles = Vec::with_capacity((n_tiles_x * n_tiles_y) as usize);

        for tile_y in 0..n_tiles_y {
            for tile_x in 0..n_tiles_x {
                let mut tile_data = [0u8; (TILE_WIDTH * TILE_WIDTH) as usize];

                for y in 0..TILE_WIDTH {
                    for x in 0..TILE_WIDTH {
                        let pixel_x = tile_x * TILE_WIDTH + x;
                        let pixel_y = tile_y * TILE_WIDTH + y;
                        let offset = pixel_y as usize * sprite.size.0 as usize + pixel_x as usize;

                        let index = palette.get_color_index(sprite.data[offset])?;
                        tile_data[(y * TILE_WIDTH + x) as usize] = index;
                    }
                }

                tiles.push(tile_data);
            }
        }

        Ok(TiledSprite {
            data: tiles,
            size: sprite.size,
        })
    }

    pub fn to_4bpp(&self) -> Vec<u8> {
        self.data
            .iter()
            .flat_map(|tile| {
                tile.chunks_exact(2)
                    .map(|p| (p[0] & 0xF) | ((p[1] & 0xF) << 4))
            })
            .collect()
    }
}
