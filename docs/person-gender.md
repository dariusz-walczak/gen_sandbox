# Person Gender: Terms and Definitions

## Person Gender {#person_gender}

A __Person Gender__ is the RDF representation of a @ref person "person’s" gender. It is expressed using the following three triples:
* @ref person_gender_triple
* @ref gender_rdf_type_triple
* @ref gender_type_triple

### Person Gender Triple {#person_gender_triple}

The __Person Gender Triple__ for a given `person` in a graph `G` is present iff `person` has at least one `gx:gender` value in `G`.

```
∃ gender : ( person, gx:gender, gender ) ∈ G
```

### Gender RDF Type Triple {#gender_rdf_type_triple}

The __Gender RDF Type Triple__ for a given `gender` in a graph `G` is present iff `gender` has type `gx:Gender` in `G`:

```
( gender, rdf:type, gx:Gender ) ∈ G
```

### Gender Type Triple {#gender_type_triple}

The __Gender Type Triple__ for a given `gender` in a graph `G` is present iff `gender` has at least one `gx:type` value in `G`.

```
∃ type : ( gender, gx:type, type ) ∈ G
```

## Known Person Gender {#known_person_gender}

A @ref person_gender is known in a graph `G` iff there exists a `gender` node such that the three @ref person_gender triples hold:

```
∃ gender, type : (
    ( person, gx:gender, gender ) ∈ G
    ∧
    ( gender, rdf:type, gx:Gender ) ∈ G
    ∧
    ( gender, gx:type, type ) ∈ G
)
```

## Unknown Person Gender {#unknown_person_gender}

A @ref person_gender is unknown in a graph `G` iff there is no gender node such that the three @ref person_gender triples hold:

```
¬∃ gender, type : (
    ( person, gx:gender, gender ) ∈ G
    ∧
    ( gender, rdf:type, gx:Gender ) ∈ G
    ∧
    ( gender, gx:type, type ) ∈ G
)
```

## Valid Person Gender {#valid_person_gender}

A @ref person_gender is valid in a graph `G` iff it is a @ref known_person_gender and the gender type is either male or female:

```
∃ gender, type : (
    ( person, gx:gender, gender ) ∈ G
    ∧
    ( gender, rdf:type, gx:Gender ) ∈ G
    ∧
    ( gender, gx:type, type ) ∈ G
    ∧
    type ∈ { gx:Male, gx:Female }
)
```

## Invalid Person Gender {#invalid_person_gender}

A @ref person_gender is invalid in a graph `G` iff it is a @ref known_person_gender and it is not a @ref valid_person_gender.

```
∃ gender, type : (
    ( person, gx:gender, gender ) ∈ G
    ∧
    ( gender, rdf:type, gx:Gender ) ∈ G
    ∧
    ( gender, gx:type, type ) ∈ G
)
∧
¬∃ gender, type : (
    ( person, gx:gender, gender ) ∈ G
    ∧
    ( gender, rdf:type, gx:Gender ) ∈ G
    ∧
    ( gender, gx:type, type ) ∈ G
    ∧
    type ∈ { gx:Male, gx:Female }
)
```

