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
* The script reads __Term Information__ from a __Filesystem Term Repository__ and transforms it into a single __JSON Document__.
* The script prints the __JSON Document__ to the standard output.
* The script works in one of two modes, the __Hierarchy Mode__ and the __Dependency Mode__.
* A __Start Term__ is a __Term__ at which the __Term Tree__ or __Term Graph__ traversal starts.
** __Start Terms__ are specified using the `--term` option.
** The __Start Term__ is at depth 1 of both the __Term Subtree__ and the __Term Graph__ traversals.
* A __Reference Path__ is a sequence of __Reference Relations__ starting at a __Start Term__.
* The __Hierarchy Mode__ is the default.
* In the __Hierarchy Mode__ the script exports the __Term Hierarchy Information__.
** The __Hierarchy Mode__ is triggered by the `--hierarchy` option.
** The __Term Hierarchy Information__ scope is controlled using __Start Terms__ and the __Tree Depth Limit__.
** The __Tree Depth Limit__ is specified using the `--max-tree-depth` option.
** In the __Hierarchy Mode__ the script traverses one or more __Term Subtrees__ using __Parent-Child Relations__.
** In the __Hierarchy Mode__ __Term Subtrees__ start at the specified __Start Terms__.
** In the __Hierarchy Mode__ the __Term Subtree__ starts at the __Tree Root__ when no __Start Term__ is specified.
** In the __Hierarchy Mode__ the script stops the traversal of a __Term Subtree__ when the __Tree Depth Limit__ is reached.
** In the __Hierarchy Mode__ the script outputs one or more __Term Subtrees__.
* In the __Dependency Mode__ the script exports the __Term Dependency Information__.
** The __Dependency Mode__ is triggered by the `--dependency` option.
** The __Term Dependency Information__ scope is controlled using __Start Terms__ and the __Reference Depth Limit__.
** The __Reference Depth Limit__ is specified using the `--max-reference-depth` option.
** In the __Dependency Mode__ the script traverses one or more __Reference Paths__ using __Reference Relations__.
** In the __Dependency Mode__ __Reference Paths__ start at the specified __Start Terms__.
** In the __Dependency Mode__ the script stops the traversal of a __Reference Path__ when the __Reference Depth Limit__ is reached.
** In the __Dependency Mode__ at least one __Start Term__ has to be specified.
** In the __Dependency Mode__ the script outputs a flat list of __Terms__.
