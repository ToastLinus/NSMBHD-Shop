# NSMBHD Shop

A native Nintendo 3DS homebrew shop front-end for New Super Mario Bros. DS and New Super Mario Bros. 2 community mods.

## Current status
This is the first buildable project foundation. It contains a native 3DS application shell, a JSON catalog format, and a GitHub Actions build that produces `NSMBHD-Shop.cia`.

The catalog is intended for creator-authorized downloads: project pages, patch files, and CIA packages that the project authors are permitted to distribute. It must not host or redistribute Nintendo game data or modified game copies without authorization. For mods that require an original game, the shop should link to the creator's patch and explain that the user must patch their own legally obtained copy.

## Build
Push to `main` or run the **Build 3DS CIA** workflow manually. The workflow builds with devkitPro and uploads `NSMBHD-Shop.cia` as an Actions artifact.

## Catalog
Edit `catalog/catalog.json`. Each entry can include:
- `title`, `system` (`DS` or `3DS`), `description`, `version`
- `project_url` for the creator's official project page
- `download_url` only for files the creator has authorized for redistribution
- `download_type` such as `patch` or `cia`

Do not add ROMs or Nintendo game data.
