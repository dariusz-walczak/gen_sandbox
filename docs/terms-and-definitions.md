## Described Resource

An IRI `res` is a **Described Resource** in a graph `G` iff there exists at least one RDF triple in `G` with `res` in the subject position, i.e.:

```
∃ pred, obj : ( res, pred, obj ) ∈ G
```

## Referenced Resource

An IRI `res` is a **Referenced Resource** in a graph `G` iff there exists at least one RDF triple in `G` with `res` in the object position, i.e.:

```
∃ subj, pred : ( subj, pred, res ) ∈ G
```

## Stubbed Resource

An IRI `res` is a **Stubbed Resource** in a graph `G` iff it is **Referenced** but not **Described** in `G`, i.e.:

```
(∃ subj, pred : ( subj, pred, res ) ∈ G) ∧ ¬(∃ pred, obj : ( res, pred, obj ) ∈ G)
```

## Typed Resource

An IRI `res` is a **Typed Resource** in a graph `G` iff there exists at least one RDF triple in `G` with `res` in the subject position and the `rdf:type` in the predicate position, i.e.:

```
∃ obj : ( res, rdf:type, obj ) ∈ G
```

## Untyped Resource

An IRI `res` is an **Untyped Resource** in a graph `G` iff it is **Described** but not **Typed** in `G`, i.e.:

```
(∃ pred, obj : ( res, pred, obj ) ∈ G) ∧ ¬(∃ obj : ( res, rdf:type, obj ) ∈ G)
```
