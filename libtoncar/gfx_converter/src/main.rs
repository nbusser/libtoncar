use asefile::AsepriteFile;
use std::{env, path::Path};

fn convert_ase(filepath: &Path) -> Result<(), Box<dyn std::error::Error>> {
    let _ase = AsepriteFile::read_file(filepath)?;
    Ok(())
}

fn main() {
    let filename = match env::args().nth(1) {
        Some(f) => f,
        None => {
            eprintln!("Usage: converter <aseprite filepath>");
            std::process::exit(1);
        }
    };

    let _ = convert_ase(Path::new(&filename));
    ()
}
