# Source Scenario 01: Polish XIX-Century Birth Registry {#sources_scenario_01}

Rename to "Source Scenario: Polish XIX-Century Birth Register"

## Overview

This document describes a specific type of genealogical data source: a **Polish XIX-century**
**birth registry**. The genealogical data extracted from a specific registry, along with metadata
describing the registry and its contents, are stored as RDF triples in a unified knowledge base,
structured according to the schema and validation rules of the gen/sandbox RDF model. This document
serves as the foundation for defining the functional requirements of the gen/sandbox model.

## Birth Registry Description

**Note**: In the scope of the description section:

* A [Register Book](#register_book) means a Polish XIX-century register book that includes a [Birth Register](#birth_register).
* A _registry book entry_ means an entry in such a book.

### Typical XIX-Century Birth Register

#### Register Book

* ✅ A typical [Register Book](#register_book) is handwritten.
* ✅ A typical [Register Book](#register_book) is preserved and provided by an [Archive](#archive).
* ✅ A [Register Book](#register_book) consists of multiple [Pages](#book_page).
* ✅ The [Register Book](#register_book) [Pages](#book_page) or [Leafs](#book_leaf) are almost always numbered.
* ✅ A typical [Register Book](#register_book) consists of a series of prose [Register Entries](#register_entry).
* ✅ A series of [Register Entries](#register_entry) may take a concise tabular form.
* ✅ A single [Register Book](#register_book) [Page](#book_page) may include one or more [Register Entries](#register_entry), either complete or partial (started or continued on adjacent **Pages**).
* ✅ All [Register Entries](#register_entry) of a typical [Register Book](#register_book) are written in the same [Language](#language) but exceptions may occur.
  (e.g., Polish and Russian in an 1868 book from the Russian Partition).
* ✅ A [Register Book](#register_book) may be a [Digitized Register Book](#digitized_register_book).
* ✅ A [Register Book](#register_book) may include [Register Book Supplementary
  Content](#register_book_supplementary_content) and [Register Entry
  Annotations](#register_entry_annotation) which may be written in languages different from the
  language(s) of the entries.
* ✅ A typical [Register Book](#register_book) includes an index of [Register Entries](#register_entry) for each included [Register](#register).

* -> Mixed Register -> Some _birth register books_ may be shared with the deaths, and marriages registers (e.g., at
  the beginning of the 19th century).

* ✅ A typical [Register Book](#register_book) is written in one primary language.
* ✅ [Register Book Suplementary Content](#register_book_supplementary_content) may be written in languages different from the [Register Entry](#register_entry) languages.

#### Online Repository

* An [Archive](#archive) may provide an [Online Repository](#online_repository)
* An [Online Repository](#online_repository) may provide a [Digitized Register Books](#digitized_register_books)
* ✅ A digitized [Register Book](#register_book) may be provided by an [Online Repository](#online_repository) and referenced using a public URL.
* ✅ A typical [Online Repository](#online_repository) allows referencing a specific page of a digitized [Register Book](#register_book) using a public URL.
* ✅ An [Online Repository](#online_repository) may allow referencing a specific [Register Entry](#register_entry) using a public URL.

#### Register Entry

* ✅ A typical [Register Entry](#register_entry) is written in one primary language.
* ✅ A typical [Register Entry](#register_entry) consists 
* ✅ Parts of a [Register Entry](#register_entry) may written in secondary languages (e.g. names transliteration, marginal notes).
* ✅ [Register Entries](#register_entry) languages vary across registries and periods (e.g. Latin, Polish, Russian, German),
  reflecting the country and year of creation.
* ✅ A typical [Register Entry](#register_entry) is a prose describing the documented event.
* ✅ The [Register Entries](#register_entry) are typically explicitly and unambiguously numbered.
* ✅ A [Register Entry](#register_entry) may span one or more pages.
* Parts of a [Register Entry](#register_entry) may be ambiguous, unreadable, or even missing due to physical damages.
* A typical [Register Entry](#register_entry) has date of creation (registration).
* A [Register Entry](#register_entry) may have one or more marginal notes.
* A marginal note may share the date of creation and the registrar with the 
* Parts of a [Register Entry](#register_entry) may have a different date of registrationcreation and authorship (e.g. marginal notes).
* In some entries, the date of creation may be imprecise -- for example, when Gregorian and Julian
  calendar dates recorded in the same entry are contradictory.

#### Register Entry Annotation

* A [Register Entry Annotation](#register_entry_annotation) can be created together with the [Register Entry](#register_entry).
* A [Register Entry Annotation](#register_entry_annotation) can be created after the [Register Entry](#register_entry) is created.
* A [Register Entry Annotation](#register_entry_annotation) authorship may be different than the [Register Entry](#register_entry) authorship.
* A [Register Entry Correction](#register_entry_correction) may be undated.

#### Birth Entry

* ✅ A [Birth Entry](#birth_entry) describes a single [Birth Event](#birth_event).
* ✅ A [Multiple Birth Event](#multiple_birth_event) may be described by one or more [Birth Entries](#birth_entry).
* ✅ A [Birth Entry](#birth_entry) that describes a [Single Birth Event](#single_birth_event) has one [Primary Participant](#primary_participant).
* ✅ A [Birth Entry](#birth_entry) that is the only entry describing a [Multiple Birth Event](#multiple_birth_event) has multiple [Primary Participants](#primary_participant).
* ✅ A [Birth Entry](#birth_entry) that is one of many entries describing a [Multiple Birth Event](#multiple_birth_event) has one [Primary Participant](#primary_participant).
* ✅ A [Birth Entry](#birth_entry) may be associated with zero, one, or more [Secondary Participants](#secondary_participant).
* ✅ A typical [Birth Entry](#birth_entry) is signed by the [Birth Event](#birth_event) [Registrar](#registrar), typically a Parish Priest, Parish Administrator, or other official.
* ✅ Some [Birth Entries](#birth_entry) are signed by [Witnesses](#witness), in addition to the signature of the [Registrar](#registrar).
* ✅ Some signatures, including the signature of the [Registrar](#registrar), are unreadable.

* A digitized form of the [Register Entry](#register_entry) may be referenced directly using a public URL.
* A digitized form of the registry book may be available as an offline resource (provided by the organization keeping it).
* A (birth) [Register Entry](#register_entry) may include marginal notes which provide information on the [Primary Participant](#primary_participant) marriages.
* Some registry book entries may be associated with transcription and translation artifacts.

### Typical Trancription Artifact

### Typical Translation Artifact



## Birth Registry Model Requirements

### Register Book Language

* When a registry book has a single primary language, that language is the default for all its
  entries.
* A [Register Entry](#register_entry) has one primary language.
* A [Register Entry](#register_entry) has zero, one, or more secondary languages.
* When a registry book has two or more primary languages (e.g., Polish and Russian in an 1868 book
  from the Russian Partition), every entry must explicitly state its language.
* When a [Register Entry](#register_entry) language is not explicitly stated, it is derived from the registry book's single
  primary language. <!-- This is part of the data model design already -->

### Book Properties

* A registry book has one or more types (birth, death, marriage)

### Entry Properties

* A [Register Entry](#register_entry) has one primary type
* A [Register Entry](#register_entry) type is derived from the registry book type if the book has only one type
