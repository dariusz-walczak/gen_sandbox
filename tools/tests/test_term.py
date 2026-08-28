import io
import os
import typing

import pytest

import shared.term

_DATA_ROOT = os.path.join(os.path.dirname(__file__), "data")


# > Test Helpers
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

def assert_term_trees_equal(
        actual: list[shared.term.Term],
        expected: list[shared.term.Term]) -> None:
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

def make_dt_term(id: str, *children: list[shared.term.Term]):
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
        id="max_tree_depth=1",
    ),
    pytest.param(
        shared.term.Options(max_tree_depth=2),
        make_dt_expected("seen_terms", 2),
        make_dt_expected("term_trees", 2),
        id="max_tree_depth=2",
    ),
    pytest.param(
        shared.term.Options(max_tree_depth=3),
        make_dt_expected("seen_terms", 3),
        make_dt_expected("term_trees", 3),
        id="max_tree_depth=3",
    ),
    pytest.param(
        shared.term.Options(max_tree_depth=4),
        make_dt_expected("seen_terms", 4),
        make_dt_expected("term_trees", 4),
        id="max_tree_depth=4",
    ),
    pytest.param(
        shared.term.Options(max_tree_depth=5),
        make_dt_expected("seen_terms", 5),
        make_dt_expected("term_trees", 5),
        id="max_tree_depth=5",
    ),
    pytest.param(
        shared.term.Options(max_tree_depth=6),
        make_dt_expected("seen_terms", 5), # Level 5 is the greatest level in the input tree
        make_dt_expected("term_trees", 5),
        id="max_tree_depth=6",
    ),
    pytest.param(
        shared.term.Options(),
        make_dt_expected("seen_terms", 5), # Level 5 is the greatest level in the input tree
        make_dt_expected("term_trees", 5),
        id="max_tree_depth=<unlimited>",
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
    assert output_term_trees == expected_term_trees
