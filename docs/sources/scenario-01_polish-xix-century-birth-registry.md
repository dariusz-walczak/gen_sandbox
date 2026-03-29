# Source Scenario 01: Polish XIX-Century Birth Registry

## Overview

This document describes a specific type of genealogical data source: a **Polish XIX-century**
**birth registry**. The genealogical data extracted from a specific registry, along with metadata
describing the registry and its contents, are stored as RDF triples in a unified knowledge base,
structured according to the schema and validation rules of the gen/sandbox RDF model. This document
serves as the foundation for defining the functional requirements of the gen/sandbox model.

## Birth Registry Description

### Typical XIX-Century Birth Registry

* A typical source is a handwritten birth registry book.
* A typical source is made available by an organisation, such as a state or church archive.
* Some sources are available online in a digitized form and can be referenced using a public URL.
* A typical registry book consists of multiple pages.
* A typical registry book consists of a series of registry entries.
* A single entry may span one or more pages.
* A single page may include one or more entries, either complete or partial (started or continued
  on adjacent pages).
* A registry book has one or more primary languages — the language(s) used for writing its entries.
* A registry book may also include secondary languages used only for supplementary content (e.g.,
  introductions, instructions, marginal notes), not for the entries themselves.
* When a registry book has a single primary language, that language is the default for all its
  entries.
* When a registry book has two or more primary languages (e.g., Polish and Russian in an 1868 book
  from the Russian Partition), every entry must explicitly state its language.
* A registry book is typically dedicated to births, deaths, or marriages.
* Some registry books may be shared between the births, deaths, and marriages registers (e.g., at
  the beginning of the 19th century).
* A series of entries may take a tabular form.
* Registry pages or sheets are almost always numbered.
* A typical source includes an index of entries at its end.

### Typical Registry Entry

* The registry entries are typically explicitly and unambiguously numbered.
* Language is a property of each entry.
* When an entry's language is not explicitly stated, it is derived from the registry book's single
  primary language.
* Entry languages vary across registries and periods (e.g. Latin, Polish, Russian, German),
  reflecting the country and year of creation.
* Each entry in a birth registry book describes a single birth event.
* Each entry in a death registry book describes a single death event.
* Each entry in a marriage registry book describes a single marriage-related event, such as the
  publication of banns or the marriage ceremony itself.
* A typical entry is associated with one or more primary participants.
* Some entries (e.g., a certificate of an unknown person's death) have no primary participant.
* Each entry may be associated with zero, one, or more secondary participants.
* A typical entry is signed by the person creating it, typically a Parish Priest, Parish
  Administrator, or other official.
* Some entries are also signed by witnesses, in addition to the signature of the person creating
  the entry.
* Some signatures, including the signature of the person creating an entry, are unreadable.
* Entries almost always have a date of creation, though in rare cases it may be absent or
  unspecified.
* In some entries, the date of creation may be imprecise -- for example, when Gregorian and Julian
  calendar dates recorded in the same entry are contradictory.

