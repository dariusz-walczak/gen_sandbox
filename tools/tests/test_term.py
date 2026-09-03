import io
import os
import typing

import pytest

import shared.term

_DATA_ROOT = os.path.join(os.path.dirname(__file__), "data")


# > Test Helpers
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

def assert_term_trees_equal(
        actual: typing.Sequence[shared.term.Term],
        expected: typing.Sequence[shared.term.Term]) -> None:
    assert len(actual) == len(expected)
    actual_by_id = {t.id: t for t in actual}
    expected_by_id = {t.id: t for t in expected}
    assert actual_by_id.keys() == expected_by_id.keys()
    for term_id in expected_by_id:
        assert_term_equal(actual_by_id[term_id], expected_by_id[term_id])


def assert_term_equal(actual: shared.term.Term, expected: shared.term.Term) -> None:
    assert actual.id == expected.id
    assert actual.title == expected.title
    assert actual.path == expected.path
    assert actual.definition == expected.definition
    assert_term_trees_equal(actual.children, expected.children)

# > shared.term.load_from_file test
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

@pytest.mark.parametrize("text,expected", [
    # Minimal valid term — anchor header only, no definition
    (
        "# Simple Term {#simple_term}\n",
        shared.term.Term(id="simple_term", title="Simple Term", path="<test>"),
    ),
    # Term with a single-line definition
    (
        "# My Term {#my_term}\n"
        "\n"
        "A __My Term__ is a test term.\n",
        shared.term.Term(
            id="my_term", title="My Term", path="<test>",
            definition=["A __My Term__ is a test term."]),
    ),
    # Term with a multi-line definition
    (
        "# Multi Term {#multi_term}\n"
        "\n"
        "First line.\n"
        "Second line.\n",
        shared.term.Term(
            id="multi_term", title="Multi Term", path="<test>",
            definition=["First line.", "Second line."]),
    ),
    # Lines before the anchor header are ignored
    (
        "<!-- comment -->\n"
        "# Prefixed Term {#prefixed_term}\n"
        "\n"
        "Definition.\n",
        shared.term.Term(
            id="prefixed_term", title="Prefixed Term", path="<test>",
            definition=["Definition."]),
    ),
    # Lines after the follow-up header are ignored
    (
        "# Follow Term {#follow_term}\n"
        "\n"
        "Definition.\n"
        "\n"
        "## Subtopics\n"
        "\n"
        "- \\subpage other_term\n",
        shared.term.Term(
            id="follow_term", title="Follow Term", path="<test>",
            definition=["Definition."]),
    ),
    # No anchor header — returns None
    (
        "No valid header here.\n",
        None,
    ),
    # Empty-lines-only definition
    (
        "# Strip Empty Lines {#strip_empty_lines}\n"
        "\n"
        "\n"
        "\n"
        "# Another section\n"
        "\n"
        "Добрий ранок\n",
        shared.term.Term(
            id="strip_empty_lines", title="Strip Empty Lines", path="<test>",
            definition=[]),
    ),
])
def test_load_from_file(text: str, expected: shared.term.Term | None) -> None:
    result = shared.term.load_from_file(io.StringIO(text), "<test>")
    assert result == expected

# > shared.term.process_input_path > flat list cases
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

@pytest.mark.parametrize("input_options,input_path,expected_seen_terms,expected_term_trees", [
    # The path locates a directory which is includes few non-term files only (it is not a term
    #  repo)
    (
        shared.term.Options(),
        "term/non-repo-dir",
        {},
        []
    ),
    # The path locates a directory with a flat list of term files (no sub-directories)
    (
        shared.term.Options(),
        "term/flat-term-list",
        {
            "abuja": shared.term.Term(
                id="abuja", title="Abuja",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "abuja.md"),
                definition=["**Abuja** is the capital of Nigeria."]),
            "accra": shared.term.Term(
                id="accra", title="Accra",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "accra.md"),
                definition=["**Accra** is the capital of Ghana."]),
            "addis_ababa": shared.term.Term(
                id="addis_ababa", title="Addis Ababa",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "addis_ababa.md"),
                definition=["**Addis Ababa** is the capital of Ethiopia."]),
            "algiers": shared.term.Term(
                id="algiers", title="Algiers",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "algiers.md"),
                definition=["**Algiers** is the capital of Algeria."]),
            "cairo": shared.term.Term(
                id="cairo", title="Cairo",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "cairo.md"),
                definition=["**Cairo** is the capital of Egypt."]),
            "nairobi": shared.term.Term(
                id="nairobi", title="Nairobi",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "nairobi.md"),
                definition=["**Nairobi** is the capital of Kenya."]),
            "pretoria": shared.term.Term(
                id="pretoria", title="Pretoria",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "pretoria.md"),
                definition=["**Pretoria** is the capital of South Africa."]),
        },
        [
            shared.term.Term(
                id="abuja", title="Abuja",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "abuja.md"),
                definition=["**Abuja** is the capital of Nigeria."]),
            shared.term.Term(
                id="accra", title="Accra",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "accra.md"),
                definition=["**Accra** is the capital of Ghana."]),
            shared.term.Term(
                id="addis_ababa", title="Addis Ababa",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "addis_ababa.md"),
                definition=["**Addis Ababa** is the capital of Ethiopia."]),
            shared.term.Term(
                id="algiers", title="Algiers",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "algiers.md"),
                definition=["**Algiers** is the capital of Algeria."]),
            shared.term.Term(
                id="cairo", title="Cairo",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "cairo.md"),
                definition=["**Cairo** is the capital of Egypt."]),
            shared.term.Term(
                id="nairobi", title="Nairobi",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "nairobi.md"),
                definition=["**Nairobi** is the capital of Kenya."]),
            shared.term.Term(
                id="pretoria", title="Pretoria",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "pretoria.md"),
                definition=["**Pretoria** is the capital of South Africa."]),
        ]
    ),
    # The path locates a directory with a twice duplicated term; duplicates are skipped
    (
        shared.term.Options(),
        "term/term-duplicate-handling-1",
        {
            "first_unique": shared.term.Term(
                id="first_unique", title="First Unique",
                path=os.path.join(
                    _DATA_ROOT, "term", "term-duplicate-handling-1", "1_unique.md"),
                definition=["The __First Unique__ is one of four unique terms."]),
            "duplicate": shared.term.Term(
                id="duplicate", title="First Duplicate",
                path=os.path.join(
                    _DATA_ROOT, "term", "term-duplicate-handling-1", "2_duplicate.md"),
                definition=["The __First Duplicate__ is one of three duplicates."]),
            "second_unique": shared.term.Term(
                id="second_unique", title="Second Unique",
                path=os.path.join(
                    _DATA_ROOT, "term", "term-duplicate-handling-1", "4_unique.md"),
                definition=["The __Second Unique__ is one of four unique terms."],
                children=[
                    shared.term.Term(
                        id="third_unique", title="Third Unique",
                        path=os.path.join(
                            _DATA_ROOT, "term", "term-duplicate-handling-1", "4_unique",
                            "5_unique.md"),
                        definition=["The __Third Unique__ is one of four unique terms."]),
                ]),
            "third_unique": shared.term.Term(
                id="third_unique", title="Third Unique",
                path=os.path.join(
                    _DATA_ROOT, "term", "term-duplicate-handling-1", "4_unique", "5_unique.md"),
                definition=["The __Third Unique__ is one of four unique terms."]),
            "fourth_unique": shared.term.Term(
                id="fourth_unique", title="Fourth Unique",
                path=os.path.join(
                    _DATA_ROOT, "term", "term-duplicate-handling-1", "7_unique.md"),
                definition=["The __Fourth Unique__ is one of four unique terms."]),
        },
        [
            shared.term.Term(
                id="first_unique", title="First Unique",
                path=os.path.join(
                    _DATA_ROOT, "term", "term-duplicate-handling-1", "1_unique.md"),
                definition=["The __First Unique__ is one of four unique terms."]),
            shared.term.Term(
                id="duplicate", title="First Duplicate",
                path=os.path.join(
                    _DATA_ROOT, "term", "term-duplicate-handling-1", "2_duplicate.md"),
                definition=["The __First Duplicate__ is one of three duplicates."]),
            shared.term.Term(
                id="second_unique", title="Second Unique",
                path=os.path.join(
                    _DATA_ROOT, "term", "term-duplicate-handling-1", "4_unique.md"),
                definition=["The __Second Unique__ is one of four unique terms."],
                children=[
                    shared.term.Term(
                        id="third_unique", title="Third Unique",
                        path=os.path.join(
                            _DATA_ROOT, "term", "term-duplicate-handling-1", "4_unique",
                            "5_unique.md"),
                        definition=["The __Third Unique__ is one of four unique terms."]),
                ]),
            shared.term.Term(
                id="fourth_unique", title="Fourth Unique",
                path=os.path.join(
                    _DATA_ROOT, "term", "term-duplicate-handling-1", "7_unique.md"),
                definition=["The __Fourth Unique__ is one of four unique terms."]),
        ]
    ),
    # The path locates a directory where the second occurrence of a duplicate term also has a child
    #  term; both the duplicate and its child are ignored.
    (
        shared.term.Options(),
        "term/term-duplicate-handling-2",
        {
            "first_unique": shared.term.Term(
                id="first_unique", title="First Unique",
                path=os.path.join(
                    _DATA_ROOT, "term", "term-duplicate-handling-2", "1_unique.md"),
                definition=["The __First Unique__ is one of four unique terms."]),
            "duplicate": shared.term.Term(
                id="duplicate", title="First Duplicate",
                path=os.path.join(
                    _DATA_ROOT, "term", "term-duplicate-handling-2", "2_duplicate.md"),
                definition=["The __First Duplicate__ is one of three duplicates."]),
        },
        [
            shared.term.Term(
                id="first_unique", title="First Unique",
                path=os.path.join(
                    _DATA_ROOT, "term", "term-duplicate-handling-2", "1_unique.md"),
                definition=["The __First Unique__ is one of four unique terms."]),
            shared.term.Term(
                id="duplicate", title="First Duplicate",
                path=os.path.join(
                    _DATA_ROOT, "term", "term-duplicate-handling-2", "2_duplicate.md"),
                definition=["The __First Duplicate__ is one of three duplicates."]),
        ]
    ),
])
def test_process_input_path_normal_cases(
        input_options: shared.term.Options,
        input_path: str,
        expected_seen_terms: dict[str, shared.term.Term],
        expected_term_trees: list[shared.term.Term]) -> None:

    input_term_path = os.path.join(_DATA_ROOT, input_path)
    output_seen_terms: dict[str, shared.term.Term] = {}

    output_term_trees = shared.term.process_input_path(
        input_options, output_seen_terms, input_term_path)

    assert output_seen_terms == expected_seen_terms
    assert output_term_trees == expected_term_trees


# > shared.term.process_input_path > deep tree cases (including tree depth limit)
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

def make_dt_term(id: str, *children: shared.term.Term) -> shared.term.Term:
    base_path = os.path.join(_DATA_ROOT, "term", "deep-tree")

    match id:
        #  Level 1: Continents
        # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
        case "africa":
            term = shared.term.Term(
                id="africa", title="Africa",
                path=os.path.join(base_path, "africa.md"),
                definition=[
                    "__Africa__ is a continent."
                ]
            )
        case "europe":
            term = shared.term.Term(
                id="europe", title="Europe",
                path=os.path.join(base_path, "europe.md"),
                definition=[
                    "__Europe__ is a continent."
                ]
            )

        #  Level 2: Countries
        # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
        case "burkina_faso":
            term = shared.term.Term(
                id="burkina_faso", title="Burkina Faso",
                path=os.path.join(base_path, "africa", "burkina_faso.md"),
                definition=[
                    "__Burkina Faso__ is a country in [Africa](#africa)."
                ]
            )
        case "ethiopia":
            term = shared.term.Term(
                id="ethiopia", title="Ethiopia",
                path=os.path.join(base_path, "africa", "ethiopia.md"),
                definition=[
                    "__Ethiopia__ is a country in [Africa](#africa)."
                ]
            )
        case "france":
            term = shared.term.Term(
                id="france", title="France",
                path=os.path.join(base_path, "europe", "france.md"),
                definition=[
                    "__France__ is a country in __[Europe](#europe)__."
                ]
            )
        case "lesotho":
            term = shared.term.Term(
                id="lesotho", title="Lesotho",
                path=os.path.join(base_path, "africa", "lesotho.md"),
                definition=[
                    "__Lesotho__ is a country in [Africa](#africa)."
                ]
            )
        case "moldova":
            term = shared.term.Term(
                id="moldova", title="Moldova",
                path=os.path.join(base_path, "europe", "moldova.md"),
                definition=[
                    "__Moldova__ is a country in [Europe](#europe)."
                ]
            )

        #  Level 3: Regions and Districts
        # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
        case "berea_district":
            term = shared.term.Term(
                id="berea_district", title="Berea District",
                path=os.path.join(base_path, "africa", "lesotho", "berea.md"),
                definition=[
                    "__Berea District__ is a district in [Lesotho](#lesotho)."
                ]
            )
        case "ile_de_france":
            term = shared.term.Term(
                id="ile_de_france", title="Île-de-France",
                path=os.path.join(base_path, "europe", "france", "ile_de_france.md"),
                definition=[
                    "__Île-de-France__ is a region of France."
                ]
            )
        case "mokhotlong_district":
            term = shared.term.Term(
                id="mokhotlong_district", title="Mokhotlong District",
                path=os.path.join(base_path, "africa", "lesotho", "mokhotlong.md"),
                definition=[
                    "__Mokhotlong District__ is a district in [Lesotho](#lesotho)."
                ]
            )
        case "nouvelle_aquitaine":
            term = shared.term.Term(
                id="nouvelle_aquitaine", title="Nouvelle-Aquitaine",
                path=os.path.join(base_path, "europe", "france", "nouvelle_aquitaine.md"),
                definition=[
                    "__Nouvelle-Aquitaine__ is a region in [France](#france)."
                ]
            )

        #  Level 4: Departments, Community Councils, and Districts
        # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
        case "kueneng_community_council":
            term = shared.term.Term(
                id="kueneng_community_council", title="Kueneng Community Council",
                path=os.path.join(
                    base_path, "africa", "lesotho", "berea", "kueneng.md"
                ),
                definition=[
                    "__Kueneng Community Council__ is a community council in the [Berea District]"
                    "(#berea_district)."
                ]
            )
        case "essonne":
            term = shared.term.Term(
                id="essonne", title="Essonne",
                path=os.path.join(
                    base_path, "europe", "france", "ile_de_france", "essonne.md"
                ),
                definition=[
                    "__Essonne__ is a department in the [Île-de-France](#ile_de_france) region."
                ]
            )
        case "seine_saint_denis":
            term = shared.term.Term(
                id="seine_saint_denis", title="Seine-Saint-Denis",
                path=os.path.join(
                    base_path, "europe", "france", "ile_de_france", "seine_saint_denis.md"
                ),
                definition=[
                    "__Seine-Saint-Denis__ is a department in the [Île-de-France](#ile_de_france)"
                    " region."
                ]
            )
        case "dordogne":
            term = shared.term.Term(
                id="dordogne", title="Dordogne",
                path=os.path.join(
                    base_path, "europe", "france", "nouvelle_aquitaine", "dordogne.md"
                ),
                definition=[
                    "__Dordogne__ is a department in the [Nouvelle-Aquitaine](#nouvelle_aquitaine)"
                    " region."
                ]
            )
        case "gironde":
            term = shared.term.Term(
                id="gironde", title="Gironde",
                path=os.path.join(
                    base_path, "europe", "france", "nouvelle_aquitaine", "gironde.md"
                ),
                definition=[
                    "__Gironde__ is a department in the [Nouvelle-Aquitaine](#nouvelle_aquitaine)"
                    " region."
                ]
            )
        case "cahul_district":
            term = shared.term.Term(
                id="cahul_district", title="Cahul District",
                path=os.path.join(
                    base_path, "europe", "moldova", "cahul.md"
                ),
                definition=[
                    "__Cahul District__ is a district in [Moldova](#moldova)."
                ]
            )
        case "hincesti_district":
            term = shared.term.Term(
                id="hincesti_district", title="Hîncești District",
                path=os.path.join(
                    base_path, "europe", "moldova", "hincesti.md"
                ),
                definition=[
                    "__Hîncești District__ is a district in [Moldova](#moldova)."
                ]
            )
        case "singerei_district":
            term = shared.term.Term(
                id="singerei_district", title="Sîngerei District",
                path=os.path.join(
                    base_path, "europe", "moldova", "singerei.md"
                ),
                definition=[
                    "__Sîngerei District__ is a district in [Moldova](#moldova)."
                ]
            )
        case "ungheni_district":
            term = shared.term.Term(
                id="ungheni_district", title="Ungheni District",
                path=os.path.join(
                    base_path, "europe", "moldova", "ungheni.md"
                ),
                definition=[
                    "__Ungheni District__ is a district in [Moldova](#moldova)."
                ]
            )

        #  Level 5: Communes and Villages
        # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
        case "bela_bela":
            term = shared.term.Term(
                id="bela_bela", title="Bela-Bela",
                path=os.path.join(
                    base_path, "africa", "lesotho", "berea", "kueneng", "bela_bela.md"
                ),
                definition=[
                    "__Bela-Bela__ is a village in the [Kueneng Community Council]"
                    "(#kueneng_community_council)."
                ]
            )
        case "ha_rajone":
            term = shared.term.Term(
                id="ha_rajone", title="Ha Rajone",
                path=os.path.join(
                    base_path, "africa", "lesotho", "berea", "kueneng", "ha_rajone.md"
                ),
                definition=[
                    "__Ha Rajone__ is a village in the [Kueneng Community Council]"
                    "(#kueneng_community_council)."
                ]
            )
        case "corbeil_essonnes":
            term = shared.term.Term(
                id="corbeil_essonnes", title="Corbeil-Essonnes",
                path=os.path.join(
                    base_path, "europe", "france", "ile_de_france", "essonne",
                    "corbeil_essonnes.md"
                ),
                definition=[
                    "__Corbeil-Essonnes__ is a commune in the [Essonne](#essonne) department."
                ]
            )
        case "evry_courcouronnes":
            term = shared.term.Term(
                id="evry_courcouronnes", title="Évry-Courcouronnes",
                path=os.path.join(
                    base_path, "europe", "france", "ile_de_france", "essonne",
                    "evry_courcouronnes.md"
                ),
                definition=[
                    "__Évry-Courcouronnes__ is a commune in the [Essonne](#essonne) department."
                ]
            )
        case "palaiseau":
            term = shared.term.Term(
                id="palaiseau", title="Palaiseau",
                path=os.path.join(
                    base_path, "europe", "france", "ile_de_france", "essonne", "palaiseau.md"
                ),
                definition=[
                    "__Palaiseau__ is a commune in the [Essonne](#essonne) department."
                ]
            )
        case "montreuil":
            term = shared.term.Term(
                id="montreuil", title="Montreuil",
                path=os.path.join(
                    base_path, "europe", "france", "ile_de_france", "seine_saint_denis",
                    "montreuil.md"
                ),
                definition=[
                    "__Montreuil__ is a commune in the [Seine-Saint-Denis](#seine_saint_denis)"
                    " department."
                ]
            )
        case "noisy_le_grand":
            term = shared.term.Term(
                id="noisy_le_grand", title="Noisy-le-Grand",
                path=os.path.join(
                    base_path, "europe", "france", "ile_de_france", "seine_saint_denis",
                    "noisy_le_grand.md"
                ),
                definition=[
                    "__Noisy-le-Grand__ is a commune in the [Seine-Saint-Denis]"
                    "(#seine_saint_denis) department."
                ]
            )
        case "bergerac":
            term = shared.term.Term(
                id="bergerac", title="Bergerac",
                path=os.path.join(
                    base_path, "europe", "france", "nouvelle_aquitaine", "dordogne", "bergerac.md"
                ),
                definition=[
                    "__Bergerac__ is a commune in the [Dordogne](#dordogne) department."
                ]
            )
        case "perigueux":
            term = shared.term.Term(
                id="perigueux", title="Périgueux",
                path=os.path.join(
                    base_path, "europe", "france", "nouvelle_aquitaine", "dordogne", "perigueux.md"
                ),
                definition=[
                    "__Périgueux__ is a commune in the [Dordogne](#dordogne) department."
                ]
            )
        case "trelissac":
            term = shared.term.Term(
                id="trelissac", title="Trélissac",
                path=os.path.join(
                    base_path, "europe", "france", "nouvelle_aquitaine", "dordogne", "trelissac.md"
                ),
                definition=[
                    "__Trélissac__ is a commune in the [Dordogne](#dordogne) department."
                ]
            )
        case "cahul":
            term = shared.term.Term(
                id="cahul", title="Cahul",
                path=os.path.join(
                    base_path, "europe", "moldova", "cahul", "cahul.md"
                ),
                definition=[
                    "__Cahul__ is a city in the [Cahul District](#cahul_district)."
                ]
            )
        case "lebedenco":
            term = shared.term.Term(
                id="lebedenco", title="Lebedenco",
                path=os.path.join(
                    base_path, "europe", "moldova", "cahul", "lebedenco.md"
                ),
                definition=[
                    "__Lebedenco__ is a commune in the [Cahul District](#cahul_district)."
                ]
            )
        case _:
            raise Exception(f"Unknown term id: {id}")

    term.children = list(children)

    return term


def make_dt_expected(
        what: typing.Literal["seen_terms", "term_trees"],
        depth: int) -> dict[str, shared.term.Term] | list[shared.term.Term]:

    match depth:
        case 1:
            if what == "seen_terms":
                return {
                    "africa": make_dt_term("africa"),
                    "europe": make_dt_term("europe"),
                }
            else:
                return [
                    make_dt_term("africa"),
                    make_dt_term("europe"),
                ]
        case 2:
            if what == "seen_terms":
                return {
                    "africa":       make_dt_term("africa"),
                    "burkina_faso": make_dt_term("burkina_faso"),
                    "ethiopia":     make_dt_term("ethiopia"),
                    "europe":       make_dt_term("europe"),
                    "france":       make_dt_term("france"),
                    "lesotho":      make_dt_term("lesotho"),
                    "moldova":      make_dt_term("moldova"),
                }
            else:
                return [
                    make_dt_term(
                        "africa",
                        make_dt_term("burkina_faso"),
                        make_dt_term("ethiopia"),
                        make_dt_term("lesotho"),
                    ),
                    make_dt_term(
                        "europe",
                        make_dt_term("france"),
                        make_dt_term("moldova"),
                    ),
                ]
        case 3:
            if what == "seen_terms":
                return {
                    "africa":              make_dt_term("africa"),
                    "burkina_faso":        make_dt_term("burkina_faso"),
                    "ethiopia":            make_dt_term("ethiopia"),
                    "europe":              make_dt_term("europe"),
                    "france":              make_dt_term("france"),
                    "lesotho":             make_dt_term("lesotho"),
                    "moldova":             make_dt_term("moldova"),
                    "berea_district":      make_dt_term("berea_district"),
                    "mokhotlong_district": make_dt_term("mokhotlong_district"),
                    "ile_de_france":       make_dt_term("ile_de_france"),
                    "nouvelle_aquitaine":  make_dt_term("nouvelle_aquitaine"),
                    "cahul_district":      make_dt_term("cahul_district"),
                    "hincesti_district":   make_dt_term("hincesti_district"),
                    "singerei_district":   make_dt_term("singerei_district"),
                    "ungheni_district":    make_dt_term("ungheni_district"),
                }
            else:
                return [
                    make_dt_term(
                        "africa",
                        make_dt_term("burkina_faso"),
                        make_dt_term("ethiopia"),
                        make_dt_term(
                            "lesotho",
                            make_dt_term("berea_district"),
                            make_dt_term("mokhotlong_district"),
                        ),
                    ),
                    make_dt_term(
                        "europe",
                        make_dt_term(
                            "france",
                            make_dt_term("ile_de_france"),
                            make_dt_term("nouvelle_aquitaine"),
                        ),
                        make_dt_term(
                            "moldova",
                            make_dt_term("cahul_district"),
                            make_dt_term("hincesti_district"),
                            make_dt_term("singerei_district"),
                            make_dt_term("ungheni_district"),
                        ),
                    ),
                ]
        case 4:
            if what == "seen_terms":
                return {
                    "africa":                    make_dt_term("africa"),
                    "berea_district":            make_dt_term("berea_district"),
                    "burkina_faso":              make_dt_term("burkina_faso"),
                    "cahul":                     make_dt_term("cahul"),
                    "cahul_district":            make_dt_term("cahul_district"),
                    "dordogne":                  make_dt_term("dordogne"),
                    "essonne":                   make_dt_term("essonne"),
                    "ethiopia":                  make_dt_term("ethiopia"),
                    "europe":                    make_dt_term("europe"),
                    "france":                    make_dt_term("france"),
                    "gironde":                   make_dt_term("gironde"),
                    "hincesti_district":         make_dt_term("hincesti_district"),
                    "ile_de_france":             make_dt_term("ile_de_france"),
                    "kueneng_community_council": make_dt_term("kueneng_community_council"),
                    "lebedenco":                 make_dt_term("lebedenco"),
                    "lesotho":                   make_dt_term("lesotho"),
                    "mokhotlong_district":       make_dt_term("mokhotlong_district"),
                    "moldova":                   make_dt_term("moldova"),
                    "nouvelle_aquitaine":        make_dt_term("nouvelle_aquitaine"),
                    "seine_saint_denis":         make_dt_term("seine_saint_denis"),
                    "singerei_district":         make_dt_term("singerei_district"),
                    "ungheni_district":          make_dt_term("ungheni_district"),
                }
            else:
                return [
                    make_dt_term(
                        "africa",
                        make_dt_term("burkina_faso"),
                        make_dt_term("ethiopia"),
                        make_dt_term(
                            "lesotho",
                            make_dt_term(
                                "berea_district",
                                make_dt_term("kueneng_community_council"),
                            ),
                            make_dt_term("mokhotlong_district"),
                        ),
                    ),
                    make_dt_term(
                        "europe",
                        make_dt_term(
                            "france",
                            make_dt_term(
                                "ile_de_france",
                                make_dt_term("essonne"),
                                make_dt_term("seine_saint_denis"),
                            ),
                            make_dt_term(
                                "nouvelle_aquitaine",
                                make_dt_term("dordogne"),
                                make_dt_term("gironde"),
                            ),
                        ),
                        make_dt_term(
                            "moldova",
                            make_dt_term(
                                "cahul_district",
                                make_dt_term("cahul"),
                                make_dt_term("lebedenco"),
                            ),
                            make_dt_term("hincesti_district"),
                            make_dt_term("singerei_district"),
                            make_dt_term("ungheni_district"),
                        ),
                    ),
                ]
        case _:
            if what == "seen_terms":
                return {
                    "africa":                    make_dt_term("africa"),
                    "berea_district":            make_dt_term("berea_district"),
                    "burkina_faso":              make_dt_term("burkina_faso"),
                    "cahul":                     make_dt_term("cahul"),
                    "cahul_district":            make_dt_term("cahul_district"),
                    "dordogne":                  make_dt_term("dordogne"),
                    "essonne":                   make_dt_term("essonne"),
                    "ethiopia":                  make_dt_term("ethiopia"),
                    "europe":                    make_dt_term("europe"),
                    "france":                    make_dt_term("france"),
                    "gironde":                   make_dt_term("gironde"),
                    "hincesti_district":         make_dt_term("hincesti_district"),
                    "ile_de_france":             make_dt_term("ile_de_france"),
                    "kueneng_community_council": make_dt_term("kueneng_community_council"),
                    "lebedenco":                 make_dt_term("lebedenco"),
                    "lesotho":                   make_dt_term("lesotho"),
                    "mokhotlong_district":       make_dt_term("mokhotlong_district"),
                    "moldova":                   make_dt_term("moldova"),
                    "nouvelle_aquitaine":        make_dt_term("nouvelle_aquitaine"),
                    "seine_saint_denis":         make_dt_term("seine_saint_denis"),
                    "singerei_district":         make_dt_term("singerei_district"),
                    "ungheni_district":          make_dt_term("ungheni_district"),
                    "bela_bela":                 make_dt_term("bela_bela"),
                    "ha_rajone":                 make_dt_term("ha_rajone"),
                    "corbeil_essonnes":          make_dt_term("corbeil_essonnes"),
                    "evry_courcouronnes":        make_dt_term("evry_courcouronnes"),
                    "palaiseau":                 make_dt_term("palaiseau"),
                    "montreuil":                 make_dt_term("montreuil"),
                    "noisy_le_grand":            make_dt_term("noisy_le_grand"),
                    "bergerac":                  make_dt_term("bergerac"),
                    "perigueux":                 make_dt_term("perigueux"),
                    "trelissac":                 make_dt_term("trelissac"),
                }
            else:
                return [
                    make_dt_term(
                        "africa",
                        make_dt_term("burkina_faso"),
                        make_dt_term("ethiopia"),
                        make_dt_term(
                            "lesotho",
                            make_dt_term(
                                "berea_district",
                                make_dt_term(
                                    "kueneng_community_council",
                                    make_dt_term("bela_bela"),
                                    make_dt_term("ha_rajone"),
                                ),
                            ),
                            make_dt_term("mokhotlong_district"),
                        ),
                    ),
                    make_dt_term(
                        "europe",
                        make_dt_term(
                            "france",
                            make_dt_term(
                                "ile_de_france",
                                make_dt_term(
                                    "essonne",
                                    make_dt_term("corbeil_essonnes"),
                                    make_dt_term("evry_courcouronnes"),
                                    make_dt_term("palaiseau"),
                                ),
                                make_dt_term(
                                    "seine_saint_denis",
                                    make_dt_term("montreuil"),
                                    make_dt_term("noisy_le_grand"),
                                ),
                            ),
                            make_dt_term(
                                "nouvelle_aquitaine",
                                make_dt_term(
                                    "dordogne",
                                    make_dt_term("bergerac"),
                                    make_dt_term("perigueux"),
                                    make_dt_term("trelissac"),
                                ),
                                make_dt_term("gironde"),
                            ),
                        ),
                        make_dt_term(
                            "moldova",
                            make_dt_term(
                                "cahul_district",
                                make_dt_term("cahul"),
                                make_dt_term("lebedenco"),
                            ),
                            make_dt_term("hincesti_district"),
                            make_dt_term("singerei_district"),
                            make_dt_term("ungheni_district"),
                        ),
                    ),
                ]

@pytest.mark.parametrize("input_options,expected_seen_terms,expected_term_trees", [
    pytest.param(
        shared.term.Options(max_tree_depth=1),
        make_dt_expected("seen_terms", 1),
        make_dt_expected("term_trees", 1),
        id="max_tree_depth_1",
    ),
    pytest.param(
        shared.term.Options(max_tree_depth=2),
        make_dt_expected("seen_terms", 2),
        make_dt_expected("term_trees", 2),
        id="max_tree_depth_2",
    ),
    pytest.param(
        shared.term.Options(max_tree_depth=3),
        make_dt_expected("seen_terms", 3),
        make_dt_expected("term_trees", 3),
        id="max_tree_depth_3",
    ),
    pytest.param(
        shared.term.Options(max_tree_depth=4),
        make_dt_expected("seen_terms", 4),
        make_dt_expected("term_trees", 4),
        id="max_tree_depth_4",
    ),
    pytest.param(
        shared.term.Options(max_tree_depth=5),
        make_dt_expected("seen_terms", 5),
        make_dt_expected("term_trees", 5),
        id="max_tree_depth_5",
    ),
    pytest.param(
        shared.term.Options(max_tree_depth=6),
        make_dt_expected("seen_terms", 5), # Level 5 is the greatest level in the input tree
        make_dt_expected("term_trees", 5),
        id="max_tree_depth_6",
    ),
    pytest.param(
        shared.term.Options(),
        make_dt_expected("seen_terms", 5), # Level 5 is the greatest level in the input tree
        make_dt_expected("term_trees", 5),
        id="max_tree_depth_unlimited",
    ),
])
def test_process_input_path_max_tree_depth(
        input_options: shared.term.Options,
        expected_seen_terms: dict[str, shared.term.Term],
        expected_term_trees: list[shared.term.Term]) -> None:

    input_term_path = os.path.join(_DATA_ROOT, "term", "deep-tree")
    output_seen_terms: dict[str, shared.term.Term] = {}

    output_term_trees = shared.term.process_input_path(
        input_options, output_seen_terms, input_term_path)

    # Terms returned in the `output_seen_terms` lookup table may or may not include children (i.e.,
    # hierarchy) information, as it is currently not needed for anything. Consequently, this test
    # replaces the children field of each seen term to ensure it does not influence the test
    # outcome.
    adapted_seen_terms = {
        term_id: term.model_copy(update={"children": []})
        for term_id, term in output_seen_terms.items()}

    assert adapted_seen_terms == expected_seen_terms
    assert_term_trees_equal(output_term_trees, expected_term_trees)


# > shared.term.extract_referenced_terms > deep graph cases (including distance limit)
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

def make_dg_term(id: str, *children: shared.term.Term) -> shared.term.Term:
    base_path = os.path.join(_DATA_ROOT, "term", "deep-graph")

    match id:
        case "albania":
            term = shared.term.Term(
                id="albania", title="Albania",
                path=os.path.join(base_path, "europe", "albania.md"),
                definition=[
                    "__Albania__ is a country in [Europe](#europe) that borders [Greece](#greece),"
                    " [Montenegro](#montenegro), [North Macedonia](#north_macedonia), and"
                    " [Kosovo](#kosovo)."
                ]
            )
        case "andorra":
            term = shared.term.Term(
                id="andorra", title="Andorra",
                path=os.path.join(base_path, "europe", "andorra.md"),
                definition=[
                    "__Andorra__ is a country in [Europe](#europe) that borders [France](#france)"
                    " and [Spain](#spain)."
                ]
            )
        case "armenia":
            term = shared.term.Term(
                id="armenia", title="Armenia",
                path=os.path.join(base_path, "europe", "armenia.md"),
                definition=[
                    "__Armenia__ is a country in [Europe](#europe) that borders [Turkey](#turkey),"
                    " __Iran__, [Azerbaijan](#azerbaijan), and [Georgia](#georgia)."
                ]
            )
        case "austria":
            term = shared.term.Term(
                id="austria", title="Austria",
                path=os.path.join(base_path, "europe", "austria.md"),
                definition=[
                    "__Austria__ is a country in [Europe](#europe) that borders [Germany]"
                    "(#germany), [Czechia](#czechia), [Slovakia](#slovakia), [Hungary](#hungary),"
                    " [Slovenia](#slovenia), [Italy](#italy), [Switzerland](#switzerland), and"
                    " [Liechtenstein](#liechtenstein)."
                ]
            )
        case "azerbaijan":
            term = shared.term.Term(
                id="azerbaijan", title="Azerbaijan",
                path=os.path.join(base_path, "europe", "azerbaijan.md"),
                definition=[
                    "__Azerbaijan__ is a country in [Europe](#europe) that borders [Georgia]"
                    "(#georgia), [Armenia](#armenia), __Iran__, and [Russia](#russia)."
                ]
            )
        case "belarus":
            term = shared.term.Term(
                id="belarus", title="Belarus",
                path=os.path.join(base_path, "europe", "belarus.md"),
                definition=[
                    "__Belarus__ is a country in [Europe](#europe) that borders [Russia](#russia),"
                    " [Ukraine](#ukraine), [Poland](#poland), [Lithuania](#lithuania), and"
                    " [Latvia](#latvia)."
                ]
            )
        case "belgium":
            term = shared.term.Term(
                id="belgium", title="Belgium",
                path=os.path.join(base_path, "europe", "belgium.md"),
                definition=[
                    "__Belgium__ is a country in [Europe](#europe) that borders [France](#france),"
                    " [Germany](#germany), [Luxembourg](#luxembourg), and [Netherlands]"
                    "(#netherlands)."
                ]
            )
        case "bosnia_and_herzegovina":
            term = shared.term.Term(
                id="bosnia_and_herzegovina", title="Bosnia and Herzegovina",
                path=os.path.join(base_path, "europe", "bosnia_and_herzegovina.md"),
                definition=[
                    "__Bosnia and Herzegovina__ is a country in [Europe](#europe) that borders"
                    " [Croatia](#croatia), [Serbia](#serbia), and [Montenegro](#montenegro)."
                ]
            )
        case "bulgaria":
            term = shared.term.Term(
                id="bulgaria", title="Bulgaria",
                path=os.path.join(base_path, "europe", "bulgaria.md"),
                definition=[
                    "__Bulgaria__ is a country in [Europe](#europe) that borders [Romania]"
                    "(#romania), [Serbia](#serbia), [North Macedonia](#north_macedonia), [Greece]"
                    "(#greece), and [Turkey](#turkey)."
                ]
            )
        case "croatia":
            term = shared.term.Term(
                id="croatia", title="Croatia",
                path=os.path.join(base_path, "europe", "croatia.md"),
                definition=[
                    "__Croatia__ is a country in [Europe](#europe) that borders [Slovenia]"
                    "(#slovenia), [Hungary](#hungary), [Serbia](#serbia), [Bosnia and Herzegovina]"
                    "(#bosnia_and_herzegovina), and [Montenegro](#montenegro)."
                ]
            )
        case "cyprus":
            term = shared.term.Term(
                id="cyprus", title="Cyprus",
                path=os.path.join(base_path, "europe", "cyprus.md"),
                definition=[
                    "__Cyprus__ is a country in [Europe](#europe) that has no land borders."
                ]
            )
        case "czechia":
            term = shared.term.Term(
                id="czechia", title="Czechia",
                path=os.path.join(base_path, "europe", "czechia.md"),
                definition=[
                    "__Czechia__ is a country in [Europe](#europe) that borders [Germany]"
                    "(#germany), [Austria](#austria), [Slovakia](#slovakia), and [Poland]"
                    "(#poland)."
                ]
            )
        case "denmark":
            term = shared.term.Term(
                id="denmark", title="Denmark",
                path=os.path.join(base_path, "europe", "denmark.md"),
                definition=[
                    "__Denmark__ is a country in [Europe](#europe) that borders [Germany]"
                    "(#germany)."
                ]
            )
        case "estonia":
            term = shared.term.Term(
                id="estonia", title="Estonia",
                path=os.path.join(base_path, "europe", "estonia.md"),
                definition=[
                    "__Estonia__ is a country in [Europe](#europe) that borders [Latvia](#latvia)"
                    " and [Russia](#russia)."
                ]
            )
        case "europe":
            term = shared.term.Term(
                id="europe", title="Europe",
                path=os.path.join(base_path, "europe.md"),
                definition=[
                    "__Europe__ is a continent."
                ]
            )
        case "finland":
            term = shared.term.Term(
                id="finland", title="Finland",
                path=os.path.join(base_path, "europe", "finland.md"),
                definition=[
                    "__Finland__ is a country in [Europe](#europe) that borders [Sweden](#sweden),"
                    " [Russia](#russia), and [Norway](#norway)."
                ]
            )
        case "france":
            term = shared.term.Term(
                id="france", title="France",
                path=os.path.join(base_path, "europe", "france.md"),
                definition=[
                    "__France__ is a country in [Europe](#europe) that borders [Belgium]"
                    "(#belgium), [Luxembourg](#luxembourg), [Germany](#germany), [Switzerland]"
                    "(#switzerland), [Italy](#italy), [Monaco](#monaco), [Andorra](#andorra), and"
                    " [Spain](#spain)."
                ]
            )
        case "georgia":
            term = shared.term.Term(
                id="georgia", title="Georgia",
                path=os.path.join(base_path, "europe", "georgia.md"),
                definition=[
                    "__Georgia__ is a country in [Europe](#europe) that borders [Russia](#russia),"
                    " [Azerbaijan](#azerbaijan), [Armenia](#armenia), and [Turkey](#turkey)."
                ]
            )
        case "germany":
            term = shared.term.Term(
                id="germany", title="Germany",
                path=os.path.join(base_path, "europe", "germany.md"),
                definition=[
                    "__Germany__ is a country in [Europe](#europe) that borders [Denmark]"
                    "(#denmark), [Poland](#poland), [Czechia](#czechia), [Austria](#austria),"
                    " [Switzerland](#switzerland), [France](#france), [Luxembourg](#luxembourg),"
                    " [Belgium](#belgium), and [Netherlands](#netherlands)."
                ]
            )
        case "greece":
            term = shared.term.Term(
                id="greece", title="Greece",
                path=os.path.join(base_path, "europe", "greece.md"),
                definition=[
                    "__Greece__ is a country in [Europe](#europe) that borders [Albania]"
                    "(#albania), [North Macedonia](#north_macedonia), [Bulgaria](#bulgaria), and"
                    " [Turkey](#turkey)."
                ]
            )
        case "hungary":
            term = shared.term.Term(
                id="hungary", title="Hungary",
                path=os.path.join(base_path, "europe", "hungary.md"),
                definition=[
                    "__Hungary__ is a country in [Europe](#europe) that borders [Austria]"
                    "(#austria), [Slovakia](#slovakia), [Ukraine](#ukraine), [Romania](#romania),"
                    " [Serbia](#serbia), [Croatia](#croatia), and [Slovenia](#slovenia)."
                ]
            )
        case "iceland":
            term = shared.term.Term(
                id="iceland", title="Iceland",
                path=os.path.join(base_path, "europe", "iceland.md"),
                definition=[
                    "__Iceland__ is a country in [Europe](#europe) that has no land borders."
                ]
            )
        case "ireland":
            term = shared.term.Term(
                id="ireland", title="Ireland",
                path=os.path.join(base_path, "europe", "ireland.md"),
                definition=[
                    "__Ireland__ is a country in [Europe](#europe) that borders [United Kingdom]"
                    "(#united_kingdom)."
                ]
            )
        case "italy":
            term = shared.term.Term(
                id="italy", title="Italy",
                path=os.path.join(base_path, "europe", "italy.md"),
                definition=[
                    "__Italy__ is a country in [Europe](#europe) that borders [France](#france),"
                    " [Switzerland](#switzerland), [Austria](#austria), [Slovenia](#slovenia),"
                    " [San Marino](#san_marino), and [Vatican City](#vatican_city)."
                ]
            )
        case "kazakhstan":
            term = shared.term.Term(
                id="kazakhstan", title="Kazakhstan",
                path=os.path.join(base_path, "europe", "kazakhstan.md"),
                definition=[
                    "__Kazakhstan__ is a country in [Europe](#europe) that borders [Russia]"
                    "(#russia), __China__, __Kyrgyzstan__, __Uzbekistan__, and __Turkmenistan__."
                ]
            )
        case "kosovo":
            term = shared.term.Term(
                id="kosovo", title="Kosovo",
                path=os.path.join(base_path, "europe", "kosovo.md"),
                definition=[
                    "__Kosovo__ is a country in [Europe](#europe) that has partial diplomatic"
                    " recognition and borders [Serbia](#serbia), [Montenegro](#montenegro),"
                    " [Albania](#albania), and [North Macedonia](#north_macedonia)."
                ]
            )
        case "latvia":
            term = shared.term.Term(
                id="latvia", title="Latvia",
                path=os.path.join(base_path, "europe", "latvia.md"),
                definition=[
                    "__Latvia__ is a country in [Europe](#europe) that borders [Estonia]"
                    "(#estonia), [Lithuania](#lithuania), [Russia](#russia), and"
                    " [Belarus](#belarus)."
                ]
            )
        case "liechtenstein":
            term = shared.term.Term(
                id="liechtenstein", title="Liechtenstein",
                path=os.path.join(base_path, "europe", "liechtenstein.md"),
                definition=[
                    "__Liechtenstein__ is a country in [Europe](#europe) that borders"
                    " [Switzerland](#switzerland) and [Austria](#austria)."
                ]
            )
        case "lithuania":
            term = shared.term.Term(
                id="lithuania", title="Lithuania",
                path=os.path.join(base_path, "europe", "lithuania.md"),
                definition=[
                    "__Lithuania__ is a country in [Europe](#europe) that borders [Poland]"
                    "(#poland), [Latvia](#latvia), [Belarus](#belarus), and [Russia](#russia)."
                ]
            )
        case "luxembourg":
            term = shared.term.Term(
                id="luxembourg", title="Luxembourg",
                path=os.path.join(base_path, "europe", "luxembourg.md"),
                definition=[
                    "__Luxembourg__ is a country in [Europe](#europe) that borders [Belgium]"
                    "(#belgium), [Germany](#germany), and [France](#france)."
                ]
            )
        case "malta":
            term = shared.term.Term(
                id="malta", title="Malta",
                path=os.path.join(base_path, "europe", "malta.md"),
                definition=[
                    "__Malta__ is a country in [Europe](#europe) that has no land borders."
                ]
            )
        case "moldova":
            term = shared.term.Term(
                id="moldova", title="Moldova",
                path=os.path.join(base_path, "europe", "moldova.md"),
                definition=[
                    "__Moldova__ is a country in [Europe](#europe) that borders [Romania]"
                    "(#romania) and [Ukraine](#ukraine)."
                ]
            )
        case "monaco":
            term = shared.term.Term(
                id="monaco", title="Monaco",
                path=os.path.join(base_path, "europe", "monaco.md"),
                definition=[
                    "__Monaco__ is a country in [Europe](#europe) that borders [France](#france)."
                ]
            )
        case "montenegro":
            term = shared.term.Term(
                id="montenegro", title="Montenegro",
                path=os.path.join(base_path, "europe", "montenegro.md"),
                definition=[
                    "__Montenegro__ is a country in [Europe](#europe) that borders [Serbia]"
                    "(#serbia), [Bosnia and Herzegovina](#bosnia_and_herzegovina), [Albania]"
                    "(#albania), [Croatia](#croatia), and [Kosovo](#kosovo)."
                ]
            )
        case "netherlands":
            term = shared.term.Term(
                id="netherlands", title="Netherlands",
                path=os.path.join(base_path, "europe", "netherlands.md"),
                definition=[
                    "__Netherlands__ is a country in [Europe](#europe) that borders [Germany]"
                    "(#germany) and [Belgium](#belgium)."
                ]
            )
        case "north_macedonia":
            term = shared.term.Term(
                id="north_macedonia", title="North Macedonia",
                path=os.path.join(base_path, "europe", "north_macedonia.md"),
                definition=[
                    "__North Macedonia__ is a country in [Europe](#europe) that borders [Serbia]"
                    "(#serbia), [Kosovo](#kosovo), [Albania](#albania), [Greece](#greece), and"
                    " [Bulgaria](#bulgaria)."
                ]
            )
        case "norway":
            term = shared.term.Term(
                id="norway", title="Norway",
                path=os.path.join(base_path, "europe", "norway.md"),
                definition=[
                    "__Norway__ is a country in [Europe](#europe) that borders [Sweden](#sweden),"
                    " [Finland](#finland), and [Russia](#russia)."
                ]
            )
        case "poland":
            term = shared.term.Term(
                id="poland", title="Poland",
                path=os.path.join(base_path, "europe", "poland.md"),
                definition=[
                    "__Poland__ is a country in [Europe](#europe) that borders [Germany]"
                    "(#germany), [Czechia](#czechia), [Slovakia](#slovakia), [Ukraine](#ukraine),"
                    " [Belarus](#belarus), [Lithuania](#lithuania), and [Russia](#russia)."
                ]
            )
        case "portugal":
            term = shared.term.Term(
                id="portugal", title="Portugal",
                path=os.path.join(base_path, "europe", "portugal.md"),
                definition=[
                    "__Portugal__ is a country in [Europe](#europe) that borders [Spain](#spain)."
                ]
            )
        case "romania":
            term = shared.term.Term(
                id="romania", title="Romania",
                path=os.path.join(base_path, "europe", "romania.md"),
                definition=[
                    "__Romania__ is a country in [Europe](#europe) that borders [Ukraine]"
                    "(#ukraine), [Moldova](#moldova), [Bulgaria](#bulgaria), [Serbia](#serbia),"
                    " and [Hungary](#hungary)."
                ]
            )
        case "russia":
            term = shared.term.Term(
                id="russia", title="Russia",
                path=os.path.join(base_path, "europe", "russia.md"),
                definition=[
                    "__Russia__ is a country in [Europe](#europe) that borders [Norway](#norway),"
                    " [Finland](#finland), [Estonia](#estonia), [Latvia](#latvia), [Lithuania]"
                    "(#lithuania), [Poland](#poland), [Belarus](#belarus), [Ukraine](#ukraine),"
                    " [Georgia](#georgia), [Azerbaijan](#azerbaijan), [Kazakhstan](#kazakhstan),"
                    " __China__, __Mongolia__, and __North Korea__."
                ]
            )
        case "san_marino":
            term = shared.term.Term(
                id="san_marino", title="San Marino",
                path=os.path.join(base_path, "europe", "san_marino.md"),
                definition=[
                    "__San Marino__ is a country in [Europe](#europe) that borders [Italy]"
                    "(#italy)."
                ]
            )
        case "serbia":
            term = shared.term.Term(
                id="serbia", title="Serbia",
                path=os.path.join(base_path, "europe", "serbia.md"),
                definition=[
                    "__Serbia__ is a country in [Europe](#europe) that borders [Hungary]"
                    "(#hungary), [Romania](#romania), [Bulgaria](#bulgaria), [North Macedonia]"
                    "(#north_macedonia), [Kosovo](#kosovo), [Montenegro](#montenegro), [Bosnia and"
                    " Herzegovina](#bosnia_and_herzegovina), and [Croatia](#croatia)."
                ]
            )
        case "slovakia":
            term = shared.term.Term(
                id="slovakia", title="Slovakia",
                path=os.path.join(base_path, "europe", "slovakia.md"),
                definition=[
                    "__Slovakia__ is a country in [Europe](#europe) that borders [Poland]"
                    "(#poland), [Ukraine](#ukraine), [Hungary](#hungary), [Czechia](#czechia),"
                    " and [Austria](#austria)."
                ]
            )
        case "slovenia":
            term = shared.term.Term(
                id="slovenia", title="Slovenia",
                path=os.path.join(base_path, "europe", "slovenia.md"),
                definition=[
                    "__Slovenia__ is a country in [Europe](#europe) that borders [Italy](#italy),"
                    " [Austria](#austria), [Hungary](#hungary), and [Croatia](#croatia)."
                ]
            )
        case "spain":
            term = shared.term.Term(
                id="spain", title="Spain",
                path=os.path.join(base_path, "europe", "spain.md"),
                definition=[
                    "__Spain__ is a country in [Europe](#europe) that borders [France](#france),"
                    " [Andorra](#andorra), and [Portugal](#portugal)."
                ]
            )
        case "sweden":
            term = shared.term.Term(
                id="sweden", title="Sweden",
                path=os.path.join(base_path, "europe", "sweden.md"),
                definition=[
                    "__Sweden__ is a country in [Europe](#europe) that borders [Norway](#norway)"
                    " and [Finland](#finland)."
                ]
            )
        case "switzerland":
            term = shared.term.Term(
                id="switzerland", title="Switzerland",
                path=os.path.join(base_path, "europe", "switzerland.md"),
                definition=[
                    "__Switzerland__ is a country in [Europe](#europe) that borders [France]"
                    "(#france), [Germany](#germany), [Austria](#austria), [Liechtenstein]"
                    "(#liechtenstein), and [Italy](#italy)."
                ]
            )
        case "turkey":
            term = shared.term.Term(
                id="turkey", title="Turkey",
                path=os.path.join(base_path, "europe", "turkey.md"),
                definition=[
                    "__Turkey__ is a country in [Europe](#europe) that borders [Greece](#greece),"
                    " [Bulgaria](#bulgaria), [Georgia](#georgia), [Armenia](#armenia),"
                    " [Azerbaijan](#azerbaijan), __Iran__, __Iraq__, and __Syria__."
                ]
            )
        case "ukraine":
            term = shared.term.Term(
                id="ukraine", title="Ukraine",
                path=os.path.join(base_path, "europe", "ukraine.md"),
                definition=[
                    "__Ukraine__ is a country in [Europe](#europe) that borders [Poland](#poland),"
                    " [Slovakia](#slovakia), [Hungary](#hungary), [Romania](#romania), [Moldova]"
                    "(#moldova), [Belarus](#belarus), and [Russia](#russia)."
                ]
            )
        case "united_kingdom":
            term = shared.term.Term(
                id="united_kingdom", title="United Kingdom",
                path=os.path.join(base_path, "europe", "united_kingdom.md"),
                definition=[
                    "__United Kingdom__ is a country in [Europe](#europe) that borders [Ireland]"
                    "(#ireland)."
                ]
            )
        case "vatican_city":
            term = shared.term.Term(
                id="vatican_city", title="Vatican City",
                path=os.path.join(base_path, "europe", "vatican_city.md"),
                definition=[
                    "__Vatican City__ is a country in [Europe](#europe) that borders [Italy]"
                    "(#italy)."
                ]
            )
        case _:
            raise Exception(f"Unknown term id: {id}")

    term.children = list(children)

    return term

def make_dg_term_lookup() -> dict[str, shared.term.Term]:
    return {
        "albania":                make_dg_term("albania"),
        "andorra":                make_dg_term("andorra"),
        "armenia":                make_dg_term("armenia"),
        "austria":                make_dg_term("austria"),
        "azerbaijan":             make_dg_term("azerbaijan"),
        "belarus":                make_dg_term("belarus"),
        "belgium":                make_dg_term("belgium"),
        "bosnia_and_herzegovina": make_dg_term("bosnia_and_herzegovina"),
        "bulgaria":               make_dg_term("bulgaria"),
        "croatia":                make_dg_term("croatia"),
        "cyprus":                 make_dg_term("cyprus"),
        "czechia":                make_dg_term("czechia"),
        "denmark":                make_dg_term("denmark"),
        "estonia":                make_dg_term("estonia"),
        "europe":                 make_dg_term("europe"),
        "finland":                make_dg_term("finland"),
        "france":                 make_dg_term("france"),
        "georgia":                make_dg_term("georgia"),
        "germany":                make_dg_term("germany"),
        "greece":                 make_dg_term("greece"),
        "hungary":                make_dg_term("hungary"),
        "iceland":                make_dg_term("iceland"),
        "ireland":                make_dg_term("ireland"),
        "italy":                  make_dg_term("italy"),
        "kazakhstan":             make_dg_term("kazakhstan"),
        "kosovo":                 make_dg_term("kosovo"),
        "latvia":                 make_dg_term("latvia"),
        "liechtenstein":          make_dg_term("liechtenstein"),
        "lithuania":              make_dg_term("lithuania"),
        "luxembourg":             make_dg_term("luxembourg"),
        "malta":                  make_dg_term("malta"),
        "moldova":                make_dg_term("moldova"),
        "monaco":                 make_dg_term("monaco"),
        "montenegro":             make_dg_term("montenegro"),
        "netherlands":            make_dg_term("netherlands"),
        "north_macedonia":        make_dg_term("north_macedonia"),
        "norway":                 make_dg_term("norway"),
        "poland":                 make_dg_term("poland"),
        "portugal":               make_dg_term("portugal"),
        "romania":                make_dg_term("romania"),
        "russia":                 make_dg_term("russia"),
        "san_marino":             make_dg_term("san_marino"),
        "serbia":                 make_dg_term("serbia"),
        "slovakia":               make_dg_term("slovakia"),
        "slovenia":               make_dg_term("slovenia"),
        "spain":                  make_dg_term("spain"),
        "sweden":                 make_dg_term("sweden"),
        "switzerland":            make_dg_term("switzerland"),
        "turkey":                 make_dg_term("turkey"),
        "ukraine":                make_dg_term("ukraine"),
        "united_kingdom":         make_dg_term("united_kingdom"),
        "vatican_city":           make_dg_term("vatican_city")
    }

@pytest.mark.parametrize("input_options,input_term_ids,input_term_lookup,expected_terms", [
    #   Malta
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
    pytest.param(
        shared.term.Options(max_ref_depth=1),
        ("malta",),
        make_dg_term_lookup(),
        [
            make_dg_term("malta"),
        ],
        id="malta_max_ref_depth_1",
    ),
    pytest.param(
        shared.term.Options(max_ref_depth=2),
        ("malta",),
        make_dg_term_lookup(),
        [
            make_dg_term("europe"),
            make_dg_term("malta"),
        ],
        id="malta_max_ref_depth_2",
    ),
    pytest.param(
        shared.term.Options(max_ref_depth=3),
        ("malta",),
        make_dg_term_lookup(),
        [
            make_dg_term("europe"),
            make_dg_term("malta"),
        ],
        id="malta_max_ref_depth_3",
    ),
    pytest.param(
        shared.term.Options(),
        ("malta",),
        make_dg_term_lookup(),
        [
            make_dg_term("europe"),
            make_dg_term("malta"),
        ],
        id="malta_max_ref_depth_unlimited",
    ),
    #   Cyprus + Iceland
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
    pytest.param(
        shared.term.Options(max_ref_depth=1),
        ("cyprus", "iceland"),
        make_dg_term_lookup(),
        [
            make_dg_term("cyprus"),
            make_dg_term("iceland"),
        ],
        id="cyprus_iceland_max_ref_depth_1",
    ),
    pytest.param(
        shared.term.Options(max_ref_depth=2),
        ("cyprus", "iceland"),
        make_dg_term_lookup(),
        [
            make_dg_term("cyprus"),
            make_dg_term("europe"),
            make_dg_term("iceland"),
        ],
        id="cyprus_iceland_max_ref_depth_2",
    ),
    pytest.param(
        shared.term.Options(max_ref_depth=3),
        ("cyprus", "iceland"),
        make_dg_term_lookup(),
        [
            make_dg_term("cyprus"),
            make_dg_term("europe"),
            make_dg_term("iceland"),
        ],
        id="cyprus_iceland_max_ref_depth_3",
    ),
    pytest.param(
        shared.term.Options(),
        ("cyprus", "iceland"),
        make_dg_term_lookup(),
        [
            make_dg_term("cyprus"),
            make_dg_term("europe"),
            make_dg_term("iceland"),
        ],
        id="cyprus_iceland_max_ref_depth_unlimited",
    ),
    #   Netherlands
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
    pytest.param(
        shared.term.Options(max_ref_depth=1),
        ("netherlands",),
        make_dg_term_lookup(),
        [
            make_dg_term("netherlands"), # 1
        ],
        id="netherlands_max_ref_depth_1",
    ),
    pytest.param(
        shared.term.Options(max_ref_depth=2),
        ("netherlands",),
        make_dg_term_lookup(),
        [
            make_dg_term("netherlands"), # 1
                make_dg_term("belgium"), # 2
                make_dg_term("europe"),  # 2
                make_dg_term("germany"), # 2
        ],
        id="netherlands_max_ref_depth_2",
    ),
    pytest.param(
        shared.term.Options(max_ref_depth=3),
        ("netherlands",),
        make_dg_term_lookup(),
        [
            make_dg_term("netherlands"),         # 1
                make_dg_term("belgium"),         # 2
                    make_dg_term("france"),      # 3
                    make_dg_term("luxembourg"),  # 3
                make_dg_term("europe"),          # 2
                make_dg_term("germany"),         # 2
                    make_dg_term("denmark"),     # 3
                    make_dg_term("poland"),      # 3
                    make_dg_term("czechia"),     # 3
                    make_dg_term("austria"),     # 3
                    make_dg_term("switzerland"), # 3
        ],
        id="netherlands_max_ref_depth_3",
    ),
    pytest.param(
        shared.term.Options(max_ref_depth=4),
        ("netherlands",),
        make_dg_term_lookup(),
        [
            make_dg_term("netherlands"),               # 1
                make_dg_term("belgium"),               # 2
                    make_dg_term("france"),            # 3
                        make_dg_term("italy"),         # 4
                        make_dg_term("monaco"),        # 4
                        make_dg_term("andorra"),       # 4
                        make_dg_term("spain"),         # 4
                    make_dg_term("luxembourg"),        # 3
                make_dg_term("europe"),                # 2
                make_dg_term("germany"),               # 2
                    make_dg_term("denmark"),           # 3
                    make_dg_term("poland"),            # 3
                        make_dg_term("slovakia"),      # 4
                        make_dg_term("ukraine"),       # 4
                        make_dg_term("belarus"),       # 4
                        make_dg_term("lithuania"),     # 4
                        make_dg_term("russia"),        # 4
                    make_dg_term("czechia"),           # 3
                    make_dg_term("austria"),           # 3
                        make_dg_term("hungary"),       # 4
                        make_dg_term("slovenia"),      # 4
                        make_dg_term("liechtenstein"), # 4
                    make_dg_term("switzerland"),       # 3
        ],
        id="netherlands_max_ref_depth_4",
    ),
    pytest.param(
        shared.term.Options(max_ref_depth=5),
        ("netherlands",),
        make_dg_term_lookup(),
        [
            make_dg_term("netherlands"),                  # 1
                make_dg_term("belgium"),                  # 2
                    make_dg_term("france"),               # 3
                        make_dg_term("italy"),            # 4
                            make_dg_term("san_marino"),   # 5
                            make_dg_term("vatican_city"), # 5
                        make_dg_term("monaco"),           # 4
                        make_dg_term("andorra"),          # 4
                        make_dg_term("spain"),            # 4
                            make_dg_term("portugal"),     # 5
                    make_dg_term("luxembourg"),           # 3
                make_dg_term("europe"),                   # 2
                make_dg_term("germany"),                  # 2
                    make_dg_term("denmark"),              # 3
                    make_dg_term("poland"),               # 3
                        make_dg_term("slovakia"),         # 4
                        make_dg_term("ukraine"),          # 4
                            make_dg_term("romania"),      # 5
                            make_dg_term("moldova"),      # 5
                        make_dg_term("belarus"),          # 4
                            make_dg_term("latvia"),       # 5
                        make_dg_term("lithuania"),        # 4
                        make_dg_term("russia"),           # 4
                            make_dg_term("norway"),       # 5
                            make_dg_term("finland"),      # 5
                            make_dg_term("estonia"),      # 5
                            make_dg_term("georgia"),      # 5
                            make_dg_term("azerbaijan"),   # 5
                            make_dg_term("kazakhstan"),   # 5
                    make_dg_term("czechia"),              # 3
                    make_dg_term("austria"),              # 3
                        make_dg_term("hungary"),          # 4
                            make_dg_term("serbia"),       # 5
                            make_dg_term("croatia"),      # 5
                        make_dg_term("slovenia"),         # 4
                        make_dg_term("liechtenstein"),    # 4
                    make_dg_term("switzerland"),          # 3
        ],
        id="netherlands_max_ref_depth_5",
    ),
    pytest.param(
        shared.term.Options(max_ref_depth=6),
        ("netherlands",),
        make_dg_term_lookup(),
        [
            make_dg_term("netherlands"),                                # 1
                make_dg_term("belgium"),                                # 2
                    make_dg_term("france"),                             # 3
                        make_dg_term("italy"),                          # 4
                            make_dg_term("san_marino"),                 # 5
                            make_dg_term("vatican_city"),               # 5
                        make_dg_term("monaco"),                         # 4
                        make_dg_term("andorra"),                        # 4
                        make_dg_term("spain"),                          # 4
                            make_dg_term("portugal"),                   # 5
                    make_dg_term("luxembourg"),                         # 3
                make_dg_term("europe"),                                 # 2
                make_dg_term("germany"),                                # 2
                    make_dg_term("denmark"),                            # 3
                    make_dg_term("poland"),                             # 3
                        make_dg_term("slovakia"),                       # 4
                        make_dg_term("ukraine"),                        # 4
                            make_dg_term("romania"),                    # 5
                                make_dg_term("bulgaria"),               # 6
                            make_dg_term("moldova"),                    # 5
                        make_dg_term("belarus"),                        # 4
                            make_dg_term("latvia"),                     # 5
                        make_dg_term("lithuania"),                      # 4
                        make_dg_term("russia"),                         # 4
                            make_dg_term("norway"),                     # 5
                                make_dg_term("sweden"),                 # 6
                            make_dg_term("finland"),                    # 5
                            make_dg_term("estonia"),                    # 5
                            make_dg_term("georgia"),                    # 5
                                make_dg_term("armenia"),                # 6
                                make_dg_term("turkey"),                 # 6
                            make_dg_term("azerbaijan"),                 # 5
                            make_dg_term("kazakhstan"),                 # 5
                    make_dg_term("czechia"),                            # 3
                    make_dg_term("austria"),                            # 3
                        make_dg_term("hungary"),                        # 4
                            make_dg_term("serbia"),                     # 5
                                make_dg_term("north_macedonia"),        # 6
                                make_dg_term("kosovo"),                 # 6
                                make_dg_term("montenegro"),             # 6
                                make_dg_term("bosnia_and_herzegovina"), # 6 
                            make_dg_term("croatia"),                    # 5
                        make_dg_term("slovenia"),                       # 4
                        make_dg_term("liechtenstein"),                  # 4
                    make_dg_term("switzerland"),                        # 3
        ],
        id="netherlands_max_ref_depth_6",
    ),
    pytest.param(
        shared.term.Options(max_ref_depth=7),
        ("netherlands",),
        make_dg_term_lookup(),
        [
            make_dg_term("netherlands"),                                # 1
                make_dg_term("belgium"),                                # 2
                    make_dg_term("france"),                             # 3
                        make_dg_term("italy"),                          # 4
                            make_dg_term("san_marino"),                 # 5
                            make_dg_term("vatican_city"),               # 5
                        make_dg_term("monaco"),                         # 4
                        make_dg_term("andorra"),                        # 4
                        make_dg_term("spain"),                          # 4
                            make_dg_term("portugal"),                   # 5
                    make_dg_term("luxembourg"),                         # 3
                make_dg_term("europe"),                                 # 2
                make_dg_term("germany"),                                # 2
                    make_dg_term("denmark"),                            # 3
                    make_dg_term("poland"),                             # 3
                        make_dg_term("slovakia"),                       # 4
                        make_dg_term("ukraine"),                        # 4
                            make_dg_term("romania"),                    # 5
                                make_dg_term("bulgaria"),               # 6
                                    make_dg_term("greece"),             # 7
                            make_dg_term("moldova"),                    # 5
                        make_dg_term("belarus"),                        # 4
                            make_dg_term("latvia"),                     # 5
                        make_dg_term("lithuania"),                      # 4
                        make_dg_term("russia"),                         # 4
                            make_dg_term("norway"),                     # 5
                                make_dg_term("sweden"),                 # 6
                            make_dg_term("finland"),                    # 5
                            make_dg_term("estonia"),                    # 5
                            make_dg_term("georgia"),                    # 5
                                make_dg_term("armenia"),                # 6
                                make_dg_term("turkey"),                 # 6
                            make_dg_term("azerbaijan"),                 # 5
                            make_dg_term("kazakhstan"),                 # 5
                    make_dg_term("czechia"),                            # 3
                    make_dg_term("austria"),                            # 3
                        make_dg_term("hungary"),                        # 4
                            make_dg_term("serbia"),                     # 5
                                make_dg_term("north_macedonia"),        # 6
                                    make_dg_term("albania"),            # 7
                                make_dg_term("kosovo"),                 # 6
                                make_dg_term("montenegro"),             # 6
                                make_dg_term("bosnia_and_herzegovina"), # 6
                            make_dg_term("croatia"),                    # 5
                        make_dg_term("slovenia"),                       # 4
                        make_dg_term("liechtenstein"),                  # 4
                    make_dg_term("switzerland"),                        # 3
        ],
        id="netherlands_max_ref_depth_7",
    ),
    pytest.param(
        shared.term.Options(max_ref_depth=8),
        ("netherlands",),
        make_dg_term_lookup(),
        [
            make_dg_term("netherlands"),                                # 1
                make_dg_term("belgium"),                                # 2
                    make_dg_term("france"),                             # 3
                        make_dg_term("italy"),                          # 4
                            make_dg_term("san_marino"),                 # 5
                            make_dg_term("vatican_city"),               # 5
                        make_dg_term("monaco"),                         # 4
                        make_dg_term("andorra"),                        # 4
                        make_dg_term("spain"),                          # 4
                            make_dg_term("portugal"),                   # 5
                    make_dg_term("luxembourg"),                         # 3
                make_dg_term("europe"),                                 # 2
                make_dg_term("germany"),                                # 2
                    make_dg_term("denmark"),                            # 3
                    make_dg_term("poland"),                             # 3
                        make_dg_term("slovakia"),                       # 4
                        make_dg_term("ukraine"),                        # 4
                            make_dg_term("romania"),                    # 5
                                make_dg_term("bulgaria"),               # 6
                                    make_dg_term("greece"),             # 7
                            make_dg_term("moldova"),                    # 5
                        make_dg_term("belarus"),                        # 4
                            make_dg_term("latvia"),                     # 5
                        make_dg_term("lithuania"),                      # 4
                        make_dg_term("russia"),                         # 4
                            make_dg_term("norway"),                     # 5
                                make_dg_term("sweden"),                 # 6
                            make_dg_term("finland"),                    # 5
                            make_dg_term("estonia"),                    # 5
                            make_dg_term("georgia"),                    # 5
                                make_dg_term("armenia"),                # 6
                                make_dg_term("turkey"),                 # 6
                            make_dg_term("azerbaijan"),                 # 5
                            make_dg_term("kazakhstan"),                 # 5
                    make_dg_term("czechia"),                            # 3
                    make_dg_term("austria"),                            # 3
                        make_dg_term("hungary"),                        # 4
                            make_dg_term("serbia"),                     # 5
                                make_dg_term("north_macedonia"),        # 6
                                    make_dg_term("albania"),            # 7
                                make_dg_term("kosovo"),                 # 6
                                make_dg_term("montenegro"),             # 6
                                make_dg_term("bosnia_and_herzegovina"), # 6
                            make_dg_term("croatia"),                    # 5
                        make_dg_term("slovenia"),                       # 4
                        make_dg_term("liechtenstein"),                  # 4
                    make_dg_term("switzerland"),                        # 3
        ],
        id="netherlands_max_ref_depth_8",
    ),
    #   Portugal + Armenia
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
    pytest.param(
        shared.term.Options(max_ref_depth=1),
        ("armenia", "portugal"),
        make_dg_term_lookup(),
        [
            make_dg_term("armenia"),  # 1
            make_dg_term("portugal"), # 1
        ],
        id="armenia_portugal_max_ref_depth_1",
    ),
    pytest.param(
        shared.term.Options(max_ref_depth=2),
        ("armenia", "portugal"),
        make_dg_term_lookup(),
        [
            make_dg_term("armenia"),        # 1
                make_dg_term("europe"),     # 2
                make_dg_term("turkey"),     # 2
                make_dg_term("azerbaijan"), # 2
                make_dg_term("georgia"),    # 2
            make_dg_term("portugal"),       # 1
                make_dg_term("spain"),      # 2
        ],
        id="armenia_portugal_max_ref_depth_2",
    ),
    pytest.param(
        shared.term.Options(max_ref_depth=3),
        ("armenia", "portugal"),
        make_dg_term_lookup(),
        [
            make_dg_term("armenia"),          # 1
                make_dg_term("europe"),       # 2
                make_dg_term("turkey"),       # 2
                    make_dg_term("greece"),   # 3
                    make_dg_term("bulgaria"), # 3
                make_dg_term("azerbaijan"),   # 2
                    make_dg_term("russia"),   # 3
                make_dg_term("georgia"),      # 2
            make_dg_term("portugal"),         # 1
                make_dg_term("spain"),        # 2
                    make_dg_term("france"),   # 3
                    make_dg_term("andorra"),  # 3
        ],
        id="armenia_portugal_max_ref_depth_3",
    ),
    pytest.param(
        shared.term.Options(max_ref_depth=4),
        ("armenia", "portugal"),
        make_dg_term_lookup(),
        [
            make_dg_term("armenia"),                     # 1
                make_dg_term("europe"),                  # 2
                make_dg_term("turkey"),                  # 2
                    make_dg_term("greece"),              # 3
                        make_dg_term("albania"),         # 4
                        make_dg_term("north_macedonia"), # 4
                    make_dg_term("bulgaria"),            # 3
                        make_dg_term("romania"),         # 4
                        make_dg_term("serbia"),          # 4
                make_dg_term("azerbaijan"),              # 2
                    make_dg_term("russia"),              # 3
                        make_dg_term("norway"),          # 4
                        make_dg_term("finland"),         # 4
                        make_dg_term("estonia"),         # 4
                        make_dg_term("latvia"),          # 4
                        make_dg_term("lithuania"),       # 4
                        make_dg_term("poland"),          # 4
                        make_dg_term("belarus"),         # 4
                        make_dg_term("ukraine"),         # 4
                        make_dg_term("kazakhstan"),      # 4
                make_dg_term("georgia"),                 # 2
            make_dg_term("portugal"),                    # 1
                make_dg_term("spain"),                   # 2
                    make_dg_term("france"),              # 3
                        make_dg_term("belgium"),         # 4
                        make_dg_term("luxembourg"),      # 4
                        make_dg_term("germany"),         # 4
                        make_dg_term("switzerland"),     # 4
                        make_dg_term("italy"),           # 4
                        make_dg_term("monaco"),          # 4
                    make_dg_term("andorra"),             # 3
        ],
        id="armenia_portugal_max_ref_depth_4",
    ),
    pytest.param(
        shared.term.Options(max_ref_depth=5),
        ("armenia", "portugal"),
        make_dg_term_lookup(),
        [
            make_dg_term("armenia"),                                # 1
                make_dg_term("europe"),                             # 2
                make_dg_term("turkey"),                             # 2
                    make_dg_term("greece"),                         # 3
                        make_dg_term("albania"),                    # 4
                            make_dg_term("montenegro"),             # 5
                            make_dg_term("kosovo"),                 # 5
                        make_dg_term("north_macedonia"),            # 4
                    make_dg_term("bulgaria"),                       # 3
                        make_dg_term("romania"),                    # 4
                            make_dg_term("moldova"),                # 5
                            make_dg_term("hungary"),                # 5
                        make_dg_term("serbia"),                     # 4
                            make_dg_term("bosnia_and_herzegovina"), # 5
                            make_dg_term("croatia"),                # 5
                make_dg_term("azerbaijan"),                         # 2
                    make_dg_term("russia"),                         # 3
                        make_dg_term("norway"),                     # 4
                            make_dg_term("sweden"),                 # 5
                        make_dg_term("finland"),                    # 4
                        make_dg_term("estonia"),                    # 4
                        make_dg_term("latvia"),                     # 4
                        make_dg_term("lithuania"),                  # 4
                        make_dg_term("poland"),                     # 4
                            make_dg_term("czechia"),                # 5
                            make_dg_term("slovakia"),               # 5
                        make_dg_term("belarus"),                    # 4
                        make_dg_term("ukraine"),                    # 4
                        make_dg_term("kazakhstan"),                 # 4
                make_dg_term("georgia"),                            # 2
            make_dg_term("portugal"),                               # 1
                make_dg_term("spain"),                              # 2
                    make_dg_term("france"),                         # 3
                        make_dg_term("belgium"),                    # 4
                            make_dg_term("netherlands"),            # 5
                        make_dg_term("luxembourg"),                 # 4
                        make_dg_term("germany"),                    # 4
                            make_dg_term("denmark"),                # 5
                            make_dg_term("austria"),                # 5
                        make_dg_term("switzerland"),                # 4
                            make_dg_term("liechtenstein"),          # 5
                        make_dg_term("italy"),                      # 4
                            make_dg_term("slovenia"),               # 5
                            make_dg_term("san_marino"),             # 5
                            make_dg_term("vatican_city"),           # 5
                        make_dg_term("monaco"),                     # 4
                    make_dg_term("andorra"),                        # 3
        ],
        id="armenia_portugal_max_ref_depth_5",
    ),
    pytest.param(
        shared.term.Options(max_ref_depth=6),
        ("armenia", "portugal"),
        make_dg_term_lookup(),
        [
            make_dg_term("armenia"),                                # 1
                make_dg_term("europe"),                             # 2
                make_dg_term("turkey"),                             # 2
                    make_dg_term("greece"),                         # 3
                        make_dg_term("albania"),                    # 4
                            make_dg_term("montenegro"),             # 5
                            make_dg_term("kosovo"),                 # 5
                        make_dg_term("north_macedonia"),            # 4
                    make_dg_term("bulgaria"),                       # 3
                        make_dg_term("romania"),                    # 4
                            make_dg_term("moldova"),                # 5
                            make_dg_term("hungary"),                # 5
                        make_dg_term("serbia"),                     # 4
                            make_dg_term("bosnia_and_herzegovina"), # 5
                            make_dg_term("croatia"),                # 5
                make_dg_term("azerbaijan"),                         # 2
                    make_dg_term("russia"),                         # 3
                        make_dg_term("norway"),                     # 4
                            make_dg_term("sweden"),                 # 5
                        make_dg_term("finland"),                    # 4
                        make_dg_term("estonia"),                    # 4
                        make_dg_term("latvia"),                     # 4
                        make_dg_term("lithuania"),                  # 4
                        make_dg_term("poland"),                     # 4
                            make_dg_term("czechia"),                # 5
                            make_dg_term("slovakia"),               # 5
                        make_dg_term("belarus"),                    # 4
                        make_dg_term("ukraine"),                    # 4
                        make_dg_term("kazakhstan"),                 # 4
                make_dg_term("georgia"),                            # 2
            make_dg_term("portugal"),                               # 1
                make_dg_term("spain"),                              # 2
                    make_dg_term("france"),                         # 3
                        make_dg_term("belgium"),                    # 4
                            make_dg_term("netherlands"),            # 5
                        make_dg_term("luxembourg"),                 # 4
                        make_dg_term("germany"),                    # 4
                            make_dg_term("denmark"),                # 5
                            make_dg_term("austria"),                # 5
                        make_dg_term("switzerland"),                # 4
                            make_dg_term("liechtenstein"),          # 5
                        make_dg_term("italy"),                      # 4
                            make_dg_term("slovenia"),               # 5
                            make_dg_term("san_marino"),             # 5
                            make_dg_term("vatican_city"),           # 5
                        make_dg_term("monaco"),                     # 4
                    make_dg_term("andorra"),                        # 3
        ],
        id="armenia_portugal_max_ref_depth_6",
    ),
])
def test_extract_referenced_terms_max_ref_depth(
        input_options: shared.term.Options,
        input_term_ids: typing.Sequence[str],
        input_term_lookup: dict[str, shared.term.Term],
        expected_terms: typing.Sequence[shared.term.Term]) -> None:

    output_terms = shared.term.extract_referenced_terms(
        input_options, input_term_ids, input_term_lookup)

    assert_term_trees_equal(output_terms, expected_terms)
