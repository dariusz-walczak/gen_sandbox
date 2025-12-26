## Described Resource

An IRI `res` is a **Described Resource** in a graph `G` iff there exists at least one RDF triple in `G` with `res` in the subject position, i.e.:

```
∃ pred, obj : ( res, pred, obj ) ∈ G
```

## Non-Person Resource

An IRI `res` is a **Non-Person Resource** in a graph `G` iff it is **Typed** in `G` and it is not typed as `gx:Person` in `G`, i.e.:

```
(∃ obj : ( res, rdf:type, obj ) ∈ G) ∧ ¬( (res, rdf:type, gx:Person) ∈ G )
```

## Person Resource

An IRI `res` is a **Person Resource** in a graph `G` iff it is typed as `gx:Person` in `G`, i.e.:

```
( res, rdf:type, gx:Person ) ∈ G
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

## Stated Couple

An unordered pair of IRIs `(person1, person2)` is a **Stated Couple** in a graph `G` iff there exists a `gx:Relationship` resource `rel` in `G` that states a `gx:Couple` relationship between `p1` and `p2` (in any order), i.e.:

```
∃ rel :
  ( rel, rdf:type, gx:Relationship ) ∈ G ∧
  ( rel, gx:type, gx:Couple ) ∈ G ∧
  (
    ( rel, gx:person1, person1 ) ∈ G ∧ ( rel, gx:person2, person2 ) ∈ G
    ∨
    ( rel, gx:person1, person2 ) ∈ G ∧ ( rel, gx:person2, person1 ) ∈ G
  )
```

## Inferred Couple

An unordered pair of IRIs `(person1, person2)` is an **Inferred Couple** in a graph `G` iff there exists at least one child IRI `child` and two `gx:Relationship` resources `rel1`, `rel2` in `G` such that `person1` and `person2` are both parents of `child` via `gx:ParentChild` relationships, i.e.:

```
∃ child, rel1, rel2 :
  ( rel1, rdf:type, gx:Relationship ) ∈ G ∧
  ( rel1, gx:type, gx:ParentChild ) ∈ G ∧
  ( rel1, gx:person1, person1 ) ∈ G ∧
  ( rel1, gx:person2, child  ) ∈ G ∧

  ( rel2, rdf:type, gx:Relationship ) ∈ G ∧
  ( rel2, gx:type, gx:ParentChild ) ∈ G ∧
  ( rel2, gx:person1, person2 ) ∈ G ∧
  ( rel2, gx:person2, child  ) ∈ G
```
