use thiserror::Error;

#[derive(Error, Debug)]
pub enum GfxConverterError {
    #[error("Palette already contains 16 colors")]
    MaxColorReached,

    #[error("Invalid sprite dimensions")]
    InvalidImageSize,

    #[error("IO error: {0}")]
    Io(#[from] std::io::Error),

    #[error("Aseprite parse error: {0}")]
    AsepriteParseError(String),

    #[error("Template render error: {0}")]
    TemplateError(String),
}
