mod aseprite;
mod codegen;
mod color;
mod error;
mod sprite;

use std::{env, path::Path, process};

fn main() {
    let args: Vec<String> = env::args().skip(1).collect();

    if args.is_empty() {
        eprintln!("Usage: converter [--out-dir <dir>] <aseprite filepaths>");
        process::exit(1);
    }

    let mut out_dir = String::from(".");
    let mut filepaths: Vec<String> = Vec::new();
    let mut iter = args.iter();

    while let Some(arg) = iter.next() {
        if arg == "--out-dir" {
            match iter.next() {
                Some(dir) => out_dir = dir.clone(),
                None => {
                    eprintln!("Error: --out-dir requires an argument");
                    process::exit(1);
                }
            }
        } else {
            filepaths.push(arg.clone());
        }
    }

    if filepaths.is_empty() {
        eprintln!("Error: no input files provided");
        process::exit(1);
    }

    for filepath in &filepaths {
        if let Err(e) = process_file(filepath, &out_dir) {
            eprintln!("Error processing {filepath}: {e}");
            process::exit(1);
        }
    }
}

fn process_file(filepath: &str, out_dir: &str) -> Result<(), error::GfxConverterError> {
    let (images, tags) = aseprite::parse_file(Path::new(filepath))?;
    let grouped = aseprite::group_by_tag(images, tags);
    let (palette, tag_sprites) = aseprite::build_sprites(grouped)?;
    codegen::generate(filepath, out_dir, &palette, &tag_sprites)?;
    Ok(())
}
