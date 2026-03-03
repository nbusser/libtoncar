use std::path::Path;

use asefile::AsepriteFile;

fn main() {
    let filename = Path::new("test.ase");
    let _ase = AsepriteFile::read_file(filename).expect("Aseprite file should exist");
}
