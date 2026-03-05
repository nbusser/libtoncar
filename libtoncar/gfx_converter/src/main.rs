use asefile::{AsepriteFile, Tag};
use image::DynamicImage;
use std::{
    collections::HashMap,
    env::{self},
    hash::{Hash, Hasher},
    path::Path,
    result,
};

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

    pub fn get_color_index(&mut self, color: Color) -> Result<u8, ()> {
        if color.is_transparent() {
            return Ok(0);
        }

        if let Some(index) = self.color_to_index.get(&color) {
            return Ok(*index);
        }
        if self.color_to_index.len() == (MAX_COLORS_PER_PALETTE - 1) {
            return Err(());
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
    palette: Palette16,
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

fn main() {
    if env::args().len() < 2 {
        eprintln!("Usage: converter <aseprite filepaths>");
        std::process::exit(1);
    }

    let parsed_aseprite_files: Vec<(
        String,
        Result<(Vec<DynamicImage>, Vec<Tag>), Box<dyn std::error::Error>>,
    )> = env::args()
        .skip(1)
        .map(|filepath| (filepath.clone(), parse_aseprite_file(Path::new(&filepath))))
        .collect();

    let (_parsed, errs): (Vec<_>, Vec<_>) = parsed_aseprite_files
        .into_iter()
        .partition(|(_, res)| res.is_ok());

    if errs.len() > 0 {
        eprintln!("Parsing errors:");
        errs.iter().for_each(|(filepath, error)| {
            eprintln!("File {}: \"{}\"", filepath, error.as_ref().unwrap_err());
        });
        std::process::exit(1);
    }

    ()
}
