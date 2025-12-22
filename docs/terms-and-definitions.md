## Described Resource

An IRI `res` is a **Described Resource** in a graph `G` if there exists at least one RDF triple in `G` with `res` in subject position, i.e.:

```
∃ pred, obj : ( res, pred, obj ) ∈ G
```

## Typed Resource

An IRI `res` is a **Typed Resource** in a graph `G` if there exists at least one RDF triple in `G` with `res` in the subject position and the `rdf:type` in the predicate position, i.e.:

```
∃ obj : ( res, rdf:type, obj ) ∈ G
```
