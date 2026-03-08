use asefile::Tag;
use heck::ToSnakeCase;
use minijinja::{Environment, context};
use std::{fs, path::Path};

use crate::{color::Palette16, error::GfxConverterError, sprite::TiledSprite};

pub fn sanitize_name(name: &str) -> String {
    name.chars()
        .map(|c| if c.is_ascii_alphanumeric() { c } else { '_' })
        .collect::<String>()
        .to_snake_case()
}

fn build_context<'a>(
    filename: &'a str,
    palette: &'a Palette16,
    grouped: &'a [(Tag, Vec<TiledSprite>)],
) -> minijinja::Value {
    context! {
        filename => filename,
        palette_colors => palette.get_colors().map(|color| {
            context! {
                rgb15 => color.to_rgb15(),
                type => "int",
            }
        }),
        tags => grouped.iter().map(|(tag, sprites)| {
            context! {
                name => sanitize_name(tag.name()),
                sprites => sprites.iter().map(|sprite| {
                    context! {
                        values => sprite.to_4bpp(),
                        size_x => sprite.size.0,
                        size_y => sprite.size.1,
                    }
                }).collect::<Vec<_>>()
            }
        }).collect::<Vec<_>>()
    }
}

fn render(template_str: &str, ctx: &minijinja::Value) -> Result<String, GfxConverterError> {
    let mut env = Environment::new();
    env.add_template("sprite", template_str)
        .map_err(|e| GfxConverterError::TemplateError(e.to_string()))?;
    env.get_template("sprite")
        .unwrap()
        .render(ctx)
        .map_err(|e| GfxConverterError::TemplateError(e.to_string()))
}

pub fn generate(
    filepath: &str,
    palette: &Palette16,
    grouped: &[(Tag, Vec<TiledSprite>)],
) -> Result<(), GfxConverterError> {
    let filename = sanitize_name(
        Path::new(filepath)
            .file_stem()
            .and_then(|s| s.to_str())
            .unwrap_or("output"),
    );

    let ctx = build_context(&filename, palette, grouped);

    let files = [
        (format!("{filename}.cpp"), include_str!("sprite.cpp.j2")),
        (format!("{filename}.h"), include_str!("sprite.h.j2")),
    ];

    for (output_path, template_str) in files {
        let output = render(template_str, &ctx)?;
        fs::write(output_path, output)?;
    }

    Ok(())
}
