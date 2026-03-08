use asefile::{AsepriteFile, Tag};
use image::DynamicImage;
use std::path::Path;

use crate::{
    color::Palette16,
    error::GfxConverterError,
    sprite::{Sprite, TiledSprite},
};

pub fn parse_file(filepath: &Path) -> Result<(Vec<DynamicImage>, Vec<Tag>), GfxConverterError> {
    let ase = AsepriteFile::read_file(filepath)
        .map_err(|e| GfxConverterError::AsepriteParseError(e.to_string()))?;

    let images = (0..ase.num_frames())
        .map(|f| DynamicImage::ImageRgba8(ase.frame(f).image()))
        .collect();

    let tags = (0..ase.num_tags()).map(|t| ase.tag(t).clone()).collect();

    Ok((images, tags))
}

pub fn group_by_tag(images: Vec<DynamicImage>, tags: Vec<Tag>) -> Vec<(Tag, Vec<DynamicImage>)> {
    tags.into_iter()
        .map(|tag| {
            let frames = (tag.from_frame()..=tag.to_frame())
                .map(|i| images[i as usize].clone())
                .collect();
            (tag, frames)
        })
        .collect()
}

pub fn build_sprites(
    grouped: Vec<(Tag, Vec<DynamicImage>)>,
) -> Result<(Palette16, Vec<(Tag, Vec<TiledSprite>)>), GfxConverterError> {
    let mut palette = Palette16::new();
    let mut result = Vec::with_capacity(grouped.len());

    for (tag, images) in grouped {
        let sprites = images
            .into_iter()
            .map(|img| TiledSprite::new(Sprite::new(img)?, &mut palette))
            .collect::<Result<Vec<_>, _>>()?;

        result.push((tag, sprites));
    }

    Ok((palette, result))
}
