#import "@preview/ilm:1.4.0": *

#show: ilm.with(
  title: [
    DMR radio emulation and codeplug reverse engineering
  ],
  bibliography: bibliography("references.bib"),
  author: "Hannes Matuschek, DM3MAT",
  abstract: [Reverse engineering DMR codeplugs is a cumbersome task. `anytone-emu` provides 
  tools that assist in that effort, by taking some of the pain out of it.]
)

#show raw: set text(size: 0.71em)

#include "motivation.typ"

#include "introduction.typ"

#include "pattern.typ"
