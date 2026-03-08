mod aseprite;
mod codegen;
mod color;
mod error;
mod sprite;

use std::{env, path::Path, process};

fn main() {
    let args: Vec<String> = env::args().skip(1).collect();

    if args.is_empty() {
        eprintln!("Usage: converter <aseprite filepaths>");
        process::exit(1);
    }

    for filepath in &args {
        if let Err(e) = process_file(filepath) {
            eprintln!("Error processing {filepath}: {e}");
            process::exit(1);
        }
    }
}

fn process_file(filepath: &str) -> Result<(), error::GfxConverterError> {
    let (images, tags) = aseprite::parse_file(Path::new(filepath))?;
    let grouped = aseprite::group_by_tag(images, tags);
    let (palette, tag_sprites) = aseprite::build_sprites(grouped)?;
    codegen::generate(filepath, &palette, &tag_sprites)?;
    Ok(())
}
