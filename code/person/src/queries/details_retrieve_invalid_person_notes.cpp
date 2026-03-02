#include "person/queries/details.hpp"

#include "common/data_table.hpp"
#include "common/person_utils.hpp"
#include "common/resource_utils.hpp"
#include "person/error.hpp"


namespace person
{

namespace
{

struct notes_spec
{
    // @note The order of fields is influenced by their memory alignment requirements

    /** @brief Severity (note type) for the @ref unknown_person_gender note.
     *
     *  The severity is role-dependent (e.g. higher for a parent than for the proband). */
    const common::Note::Type stubbed_person_note_type{common::Note::Type::Info};
    const common::Note::Type untyped_person_note_type{common::Note::Type::Warning};
    const common::Note::Type mistyped_person_note_type{common::Note::Type::Warning};
    const common::Note::Type invalid_person_gender_note_type{common::Note::Type::Error};
    const common::Note::Type unknown_person_gender_note_type{common::Note::Type::Info};

    /**
     * Diagnosed person role name used to:
     * * name note variable;
     * * add proper context to the error messages;
     */
    const std::string_view role{"proband"};

    const std::string_view stubbed_person_note_id{k_stubbed_proband_note_id};
    const std::string_view untyped_person_note_id{k_untyped_proband_note_id};
    const std::string_view mistyped_person_note_id{k_mistyped_proband_note_id};
    const std::string_view invalid_person_gender_note_id{k_invalid_proband_gender_note_id};
    const std::string_view unknown_person_gender_note_id{k_unknown_proband_gender_note_id};
};

common::Note create_gender_invalid_note(
    const std::shared_ptr<common::Resource>& person, const notes_spec& spec)
{
    return common::Note(
        spec.invalid_person_gender_note_type, std::string(spec.invalid_person_gender_note_id),
        {common::Variable{std::string(spec.role), person}},
        fmt::format("Invalid {} gender: {}", spec.role, person->get_uri_str()));
}

common::Note create_gender_unknown_note(
    const std::shared_ptr<common::Resource>& person, const notes_spec& spec)
{
    return common::Note(
        spec.unknown_person_gender_note_type, std::string(spec.unknown_person_gender_note_id),
        {common::Variable{std::string(spec.role), person}},
        fmt::format("Unknown {} gender: {}", spec.role, person->get_uri_str()));
}

common::Note create_stubbed_person_note(
    const std::shared_ptr<common::Resource>& person, const notes_spec& spec)
{
    return common::Note(
        spec.stubbed_person_note_type, std::string(spec.stubbed_person_note_id),
        {common::Variable{std::string(spec.role), person}},
        fmt::format("Stubbed {}: {}", spec.role, person->get_uri_str()));
}

common::Note create_untyped_person_note(
    const std::shared_ptr<common::Resource>& person, const notes_spec& spec)
{
    return common::Note(
        spec.untyped_person_note_type, std::string(spec.untyped_person_note_id),
        {common::Variable{std::string(spec.role), person}},
        fmt::format("Untyped {}: {}", spec.role, person->get_uri_str()));
}

common::Note create_mistyped_person_note(
    const std::shared_ptr<common::Resource>& person, const notes_spec& spec)
{
    return common::Note(
        spec.mistyped_person_note_type, std::string(spec.mistyped_person_note_id),
        {common::Variable{std::string(spec.role), person}},
        fmt::format("Mistyped {}: {}", spec.role, person->get_uri_str()));
}

} // anonymous namespace


void retrieve_invalid_person_notes(
    const std::shared_ptr<common::Resource>& proband, librdf_world* world, librdf_model* model,
    const notes_spec& spec, std::vector<common::Note>& notes)
{
    if (!proband)
    {
        throw person_exception(
            person_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: proband={} must satisfy !nullptr", fmt::ptr(proband)));
    }

    if (!world)
    {
        throw person_exception(
            person_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: world={} must satisfy !nullptr", fmt::ptr(world)));
    }

    if (!model)
    {
        throw person_exception(
            person_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: model={} must satisfy !nullptr", fmt::ptr(model)));
    }

    const bool described_resource_flag =
        common::ask_resource_described(proband.get(), world, model);
    const bool referenced_resource_flag =
        common::ask_resource_referenced(proband.get(), world, model);
    const bool untyped_resource_flag =
        common::ask_resource_untyped(proband.get(), world, model);
    const bool mistyped_person_flag =
        common::ask_resource_mistyped(proband.get(), common::k_person_type_uri, world, model);
    const bool typed_person_flag =
        common::ask_resource_typed(proband.get(), common::k_person_type_uri, world, model);


    const bool unknown_resource_flag = (!described_resource_flag && !referenced_resource_flag);
    const bool stubbed_resource_flag = (!described_resource_flag && referenced_resource_flag);

    const bool gender_unknown_flag =
        common::ask_person_gender_unknown(proband.get(), world, model);
    const bool gender_invalid_flag =
        common::ask_person_gender_invalid(proband.get(), world, model);

    if (unknown_resource_flag)
    {
        spdlog::error(
            "{}: The '{}' resource is unknown in the model", __func__, proband->get_uri_str());

        throw person_exception(
            person_exception::error_code::input_contract_error,
            fmt::format(
                "Precondition failure: proband={} must be known in the model",
                proband->get_uri_str()));
    }

    /** Exactly one of the stubbed, untyped, mistyped, and typed flags must be true. The opposite
     *   situation is an assumption error or a query bug, both of which are program-side issues. */
    const int resource_state_flag_sum = (
        int(stubbed_resource_flag) + int(untyped_resource_flag) + int(mistyped_person_flag) +
        int(typed_person_flag));

    if (resource_state_flag_sum != 1)
    {
        spdlog::error(
            "{}: Exactly one of the four resource state flags must be true ({} are)\n"
            "    stubbed_resource_flag={}\n"
            "    untyped_resource_flag={}\n"
            "    mistyped_person_flag={}\n"
            "    typed_person_flag={}\n",
            __func__, resource_state_flag_sum, stubbed_resource_flag, untyped_resource_flag,
            mistyped_person_flag, typed_person_flag);

        throw person_exception(
            person_exception::error_code::internal_contract_error,
            fmt::format(
                "Assumption failure: expected exactly one of the resource state flags to be true;"
                " observed {} flags being true", resource_state_flag_sum));
    }

    if (stubbed_resource_flag)
    {
        notes.emplace_back(create_stubbed_person_note(proband, spec));
    }

    if (untyped_resource_flag)
    {
        notes.emplace_back(create_untyped_person_note(proband, spec));
    }

    if (mistyped_person_flag)
    {
        notes.emplace_back(create_mistyped_person_note(proband, spec));
    }

    /** @note The `retrieve_invalid_person_notes` function shall not generate any of the gender
     *        related validity notes when the proband is a @ref stubbed_resource. */
    if (!stubbed_resource_flag)
    {
        if (gender_unknown_flag)
        {
            notes.emplace_back(create_gender_unknown_note(proband, spec));
        }
        else if (gender_invalid_flag)
        {
            notes.emplace_back(create_gender_invalid_note(proband, spec));
        }
    }
}

void retrieve_person_invalid_parent_notes(
    const common::Person* proband, librdf_world* world, librdf_model* model,
    std::vector<common::Note>& notes)
{
    const notes_spec spec = {
        .unknown_person_gender_note_type=common::Note::Type::Warning,
        .role="parent",
        .stubbed_person_note_id=k_stubbed_parent_note_id,
        .untyped_person_note_id=k_untyped_parent_note_id,
        .mistyped_person_note_id=k_mistyped_parent_note_id,
        .invalid_person_gender_note_id=k_invalid_parent_gender_note_id,
        .unknown_person_gender_note_id=k_unknown_parent_gender_note_id
    };

    const auto all_parent_resources = retrieve_person_parents(proband, world, model);

    for (const auto& parent : all_parent_resources)
    {
        retrieve_invalid_person_notes(parent, world, model, spec, notes);
    }

//    retrieve_invalid_person_notes(proband, world, model, spec, notes);
}

} // namespace person
