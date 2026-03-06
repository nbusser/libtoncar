use asefile::{AsepriteFile, Tag};
use heck::ToSnakeCase;
use image::{DynamicImage, GenericImageView, Pixel};
use minijinja::{Environment, context};
use std::{
    collections::HashMap,
    env::{self},
    error::Error,
    fmt, fs,
    hash::{Hash, Hasher},
    path::Path,
    result,
};
use thiserror::Error;

#[derive(Error, Debug)]
pub enum GfxConverterError {
    #[error("Palette already contains 16 colors")]
    MaxColorReached,
    #[error("Invalid sprite dimensions")]
    InvalidImageSize,
}

#[derive(Clone, Copy)]
pub struct Color {
    pub r: u8,
    pub g: u8,
    pub b: u8,
    pub a: u8,
}

impl Color {
    pub const fn from_rgb(r: u8, g: u8, b: u8, a: u8) -> Self {
        Color { r, g, b, a }
    }

    pub fn to_rgb15(self) -> u16 {
        let (r, g, b) = (self.r as u16, self.g as u16, self.b as u16);
        ((r >> 3) & 31) | (((g >> 3) & 31) << 5) | (((b >> 3) & 31) << 10)
    }

    pub fn is_transparent(self) -> bool {
        self.a != 255
    }
}

// Ignoring `a` when comparing colors.
impl PartialEq for Color {
    fn eq(&self, other: &Self) -> bool {
        self.r == other.r && self.g == other.g && self.b == other.b
    }
}

impl Eq for Color {}

impl Hash for Color {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.r.hash(state);
        self.g.hash(state);
        self.b.hash(state);
    }
}

const MAX_COLORS_PER_PALETTE: usize = 16;
const TRANSPARENT_COLOR: Color = Color::from_rgb(255, 0, 255, 0);

struct Palette16 {
    color_to_index: HashMap<Color, u8>,
}

impl Palette16 {
    pub fn new() -> Self {
        Palette16 {
            // Only contains 15 opaque colors.
            color_to_index: HashMap::with_capacity(MAX_COLORS_PER_PALETTE - 1),
        }
    }

    pub fn get_color_index(&mut self, color: Color) -> Result<u8, GfxConverterError> {
        if color.is_transparent() {
            return Ok(0);
        }

        if let Some(index) = self.color_to_index.get(&color) {
            return Ok(*index);
        }
        if self.color_to_index.len() == (MAX_COLORS_PER_PALETTE - 1) {
            return Err(GfxConverterError::MaxColorReached);
        }
        let index = (self.color_to_index.len() + 1) as u8;
        self.color_to_index.insert(color, index);
        Ok(index)
    }

    pub fn get_colors(&self) -> [Color; MAX_COLORS_PER_PALETTE] {
        let mut color_array = [TRANSPARENT_COLOR; MAX_COLORS_PER_PALETTE];
        for (color, &index) in &self.color_to_index {
            color_array[index as usize] = *color;
        }
        color_array
    }
}

struct ParsedAsepriteFile {
    frames: Vec<DynamicImage>,
    tags: Vec<Tag>,
}

fn valid_sprite_size(width: u32, height: u32) -> bool {
    match (width, height) {
        (8, 8) => true,
        (16, 16) => true,
        (32, 32) => true,
        (64, 64) => true,
        (16, 8) => true,
        (32, 8) => true,
        (32, 16) => true,
        (64, 32) => true,
        (8, 16) => true,
        (8, 32) => true,
        (16, 32) => true,
        (32, 64) => true,
        (_, _) => false,
    }
}

struct Sprite {
    pub data: Vec<Color>,
    pub size: (u8, u8),
}

impl Sprite {
    pub fn new(image: DynamicImage) -> Result<Self, GfxConverterError> {
        if !valid_sprite_size(image.width(), image.height()) {
            return Err(GfxConverterError::InvalidImageSize);
        }

        // Store image from top-left corner
        let mut pixels: Vec<Color> = Vec::with_capacity((image.width() * image.height()) as usize);
        for y in 0..image.height() {
            for x in 0..image.width() {
                let rgba = image.get_pixel(x, y).to_rgba();
                pixels.push(Color::from_rgb(rgba[0], rgba[1], rgba[2], rgba[3]));
            }
        }
        Ok(Sprite {
            data: pixels,
            size: (image.width() as u8, image.height() as u8),
        })
    }
}

const TILE_WIDTH: u8 = 8;

struct TiledSprite {
    data: Vec<[u8; (TILE_WIDTH * TILE_WIDTH) as usize]>,
    size: (u8, u8),
}

impl TiledSprite {
    pub fn new(sprite: Sprite, palette: &mut Palette16) -> Result<Self, GfxConverterError> {
        let n_tiles_x = sprite.size.0 / TILE_WIDTH;
        let n_tiles_y = sprite.size.1 / TILE_WIDTH;

        let mut palette_color_indexes: Vec<[u8; (TILE_WIDTH * TILE_WIDTH) as usize]> =
            Vec::with_capacity((n_tiles_x * n_tiles_y) as usize);

        for tile_y in 0..n_tiles_y {
            for tile_x in 0..n_tiles_x {
                let mut tile_data = [0u8; (TILE_WIDTH * TILE_WIDTH) as usize];

                for y in 0..TILE_WIDTH {
                    for x in 0..TILE_WIDTH {
                        let pixel_x = tile_x * TILE_WIDTH + x;
                        let pixel_y = tile_y * TILE_WIDTH + y;

                        let sprite_width = sprite.size.0 as usize;
                        let offset = pixel_y as usize * sprite_width + pixel_x as usize;

                        let color = sprite.data[offset];

                        let index = palette.get_color_index(color)?;

                        let pixel_offset = (y * TILE_WIDTH + x) as usize;
                        tile_data[pixel_offset] = index;
                    }
                }

                palette_color_indexes.push(tile_data);
            }
        }

        Ok(TiledSprite {
            data: palette_color_indexes,
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

fn build_palette_and_sprites_from_file(
    images: Vec<DynamicImage>,
) -> Result<(Palette16, Vec<TiledSprite>), GfxConverterError> {
    let mut palette = Palette16::new();
    let mut sprites = Vec::new();

    for image in images {
        let sprite = Sprite::new(image)?;
        let tiled = TiledSprite::new(sprite, &mut palette)?;
        sprites.push(tiled);
    }

    Ok((palette, sprites))
}

fn parse_aseprite_file(
    filepath: &Path,
) -> Result<(Vec<DynamicImage>, Vec<Tag>), Box<dyn std::error::Error>> {
    let ase = AsepriteFile::read_file(filepath)?;

    let mut images = Vec::new();
    let mut tags = Vec::new();

    for frame in 0..ase.num_frames() {
        let image = ase.frame(frame).image();
        images.push(DynamicImage::ImageRgba8(image))
    }

    for tag in 0..ase.num_tags() {
        tags.push(ase.tag(tag).clone())
    }

    Ok((images, tags))
}

fn sanitize_filename(filename: String) -> String {
    let s: String = filename
        .chars()
        .map(|c| if c.is_ascii_alphanumeric() { c } else { '_' })
        .collect();
    s.to_snake_case()
}

fn generate_cpp(filepath: &String, palette: &Palette16, sprites: &Vec<TiledSprite>) {
    let raw_filename = Path::new(&filepath)
        .file_stem()
        .unwrap()
        .to_str()
        .unwrap()
        .to_owned();
    let filename = sanitize_filename(raw_filename);

    let template = include_str!("sprite.cpp.j2");

    let mut env = Environment::new();
    env.add_template("sprite", template).unwrap();

    let tmpl = env.get_template("sprite").unwrap();

    let output = tmpl
        .render(context! {
            filename => filename,
            palette_name => filename + "_palette",
            palette_colors => palette.get_colors().map(|color| {
                context! {
                    rgb15 => color.to_rgb15(),
                    type => "int",
                }
            }),
            sprites => sprites.iter().map(|sprite| {
                context! {
                    name => "todo",
                    values => sprite.to_4bpp(),
                    size_x => sprite.size.0,
                    size_y => sprite.size.1
                }
            }).collect::<Vec<_>>()
        })
        .unwrap();

    fs::write("sprite.cpp", output).unwrap();
}

fn main() {
    if env::args().len() < 2 {
        eprintln!("Usage: converter <aseprite filepaths>");
        std::process::exit(1);
    }

    let parsed_aseprite_files: Vec<(String, Palette16, Vec<TiledSprite>, Vec<Tag>)> = env::args()
        .skip(1)
        .filter_map(|filepath| match parse_aseprite_file(Path::new(&filepath)) {
            Ok((images, tags)) => Some((filepath, images, tags)),
            Err(err) => {
                eprintln!("Error parsing {}: {}", filepath, err);
                std::process::exit(1);
            }
        })
        .map(
            |(filepath, images, tags)| match build_palette_and_sprites_from_file(images) {
                Ok((palette, sprites)) => (filepath, palette, sprites, tags),
                Err(err) => {
                    eprintln!("Error parsing {}: {}", filepath, err);
                    std::process::exit(1);
                }
            },
        )
        .collect();

    let _: Vec<_> = parsed_aseprite_files
        .iter()
        .map(|(filepath, palette, sprites, tags)| {
            generate_cpp(filepath, palette, sprites);
        })
        .collect();
}
