use asefile::{AsepriteFile, Tag};
use image::DynamicImage;
use std::{
    env::{self},
    path::Path,
};

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
