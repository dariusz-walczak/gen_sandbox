# Terms and Relations

* A __Term__ is described by __Term Data__.
* __Term Data__ is a composition of a __Term's__ unique identifier, title, and definition.
* __Terms__ are organized into the __Term Tree__.
** The __Term Tree__ is the hierarchy of __Terms__ formed by the __Parent-Child Relations__.
** A __Parent-Child Relation__ is a directed relation in which one __Term__ is the parent of another __Term__.
** A __Term__ has at most one parent and may have zero or more children.
** The __Term Tree__ carries the __Term Hierarchy Information__.
** The __Tree Root__ is the topmost __Term__ of the __Term Tree__.
** A __Term Subtree__ is a branch of the __Term Tree__ that starts at a specific __Term__ and includes all its descendants.
* __Terms__ may reference other __Terms__ through the __Term Graph__.
** A __Term__ may reference zero or more other __Terms__.
** A __Term__ may be referenced by zero or more other __Terms__.
** The __Term Graph__ is the set of __Reference Relations__ between the __Terms__ of the __Term Tree__.
** A __Reference Relation__ is a directed relation in which one __Term__ references another __Term__.
** The __Term Graph__ carries the __Term Dependency Information__.
* __Term Information__ is a composition of __Term Data__, __Term Hierarchy Information__, and __Term Dependency Information__.

# Term Information Representation

* __Term__ definitions carry the __Term Dependency Information__.
* The __Filesystem Term Repository__ represents the __Term Information__ as a hierarchy of files and directories.
* The __JSON Document__ represents the __Term Information__ in JSON format.

# Script Description

* The script name is `export_terms.py`.
* The script reads __Term Information__ from a __Filesystem Term Repository__ and transforms it to a JSON document.
* The script works in one of two modes, the __Hierarchy Mode__ and the __Dependency Mode__.
* The __Hierarchy Mode__ is the default.
* The script may extract the complete __Term Information__ or one or more of its parts.
* The __Term Information__ scope is controlled using the __Root Term__ option (`--term`).
* In the __Hierarchy Mode__ the script traverses one or more __Term Subtrees__ using __Parent-Child Relations__.
* In the __Hierarchy Mode__ the __Term Subtree__ starts at the __Tree Root__ when no __Root Term__ is specified.
* In the __Hierarchy Mode__ the __Term Subtrees__ start at the specified __Root Terms__.
* In the __Hierarchy Mode__ the script stops a __Term Subtree__ traverse when the __Tree Depth Limit__ is reached.
* The __Tree Depth Limit__ is controlled by the `--max-tree-depth` option.
* The __Term Tree__ depth is counted starting from the __Root Term__.
* In the __Dependency Mode__ the script traverses the __Term Graph__ using __Reference Relations__ to extract associated __Terms__.
* In the __Dependency Mode__ the script follows each __Reference Path__ starting from each __Root Term__ until a __Reference Depth Limit__ is reached.
* The __Reference Depth Limit__ is controlled by the `--max-reference-depth` option.
* The __Reference Path__ depth is counted starting from the __Root Term__.

[Idea: Rename __Reference Relations__ to __Dependency Relations__? How would the `--max-reference-depth` be called then?]
