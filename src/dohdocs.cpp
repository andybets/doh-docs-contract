#include <dohdocs.hpp>

namespace eosio {

    ACTION dohdocs::adddoc(const uint64_t item_id, const uint32_t faction_id, const uint32_t language_id, const uint64_t category_id, const name author, const std::string title, const std::string content) {
        require_auth(author);
        
        authors_table authors(get_self(), get_self().value);
        auto author_key = composite_key_128(author.value, faction_id, language_id);
        auto author_itr = authors.get(author_key, "Author not registered for the specified faction and language");

        candidatedocs_table docs(get_self(), get_self().value);
        docs.emplace(author, [&](auto& row) {
            row.id = docs.available_primary_key();
            row.item_id = item_id;
            row.faction_id = faction_id;
            row.language_id = language_id;
            row.category_id = category_id;
            row.author = author;
            row.title = title;
            row.content = content;
        });
    }

    ACTION dohdocs::deldoc(const uint64_t item_id, const uint32_t faction_id, const uint32_t language_id, const name author) {
        require_auth(author);
        
        candidatedocs_table docs(get_self(), get_self().value);
        auto doc_key = composite_key_128(item_id, faction_id, language_id);
        auto doc_itr = docs.get(doc_key, "Document not found");
        eosio::check(doc_itr.author == author, "Only the author or editor can delete the document");

        docs.erase(doc_itr);
    }

    ACTION dohdocs::publish(const uint64_t item_id, const uint32_t faction_id, const uint32_t language_id, const name editor) {
        require_auth(editor);
        
        editors_table editors(get_self(), get_self().value);
        auto editor_itr = editors.find(editor.value);
        eosio::check(editor_itr != editors.end(), "Editor not registered");

        candidatedocs_table candidate_docs(get_self(), get_self().value);
        auto candidate_key = composite_key_128(item_id, faction_id, language_id);
        auto candidate_itr = candidate_docs.get(candidate_key, "Document not found");

        publisheddocs_table published_docs(get_self(), get_self().value);
        published_docs.emplace(editor, [&](auto& row) {
            row.id = published_docs.available_primary_key();
            row.item_id = item_id;
            row.faction_id = faction_id;
            row.language_id = language_id;
            row.category_id = candidate_itr.category_id;
            row.author = candidate_itr.author;
            row.title = candidate_itr.title;
            row.content = candidate_itr.content;
            row.approved_at = current_time_point();
            row.approved_by = editor;
        });

    }

    ACTION dohdocs::unpublish(const uint64_t item_id, const uint32_t faction_id, const uint32_t language_id, const name editor) {
        require_auth(editor);
        
        editors_table editors(get_self(), get_self().value);
        auto editor_itr = editors.find(editor.value);
        eosio::check(editor_itr != editors.end(), "Editor not registered");

        publisheddocs_table published_docs(get_self(), get_self().value);
        auto pub_key = composite_key_128(item_id, faction_id, language_id);
        auto pub_itr = published_docs.get(pub_key, "Document not found");

        published_docs.erase(pub_itr);
    }

    ACTION dohdocs::regauthor(const name author, const uint32_t faction_id, const uint32_t language_id, const name editor) {
        require_auth(editor);
        
        editors_table editors(get_self(), get_self().value);
        auto editor_itr = editors.find(editor.value);
        eosio::check(editor_itr != editors.end(), "Editor not registered");

        authors_table authors(get_self(), get_self().value);
        authors.emplace(editor, [&](auto& row) {
            row.id = author.value;
            row.account = author;
            row.faction_id = faction_id;
            row.language_id = language_id;
        });
    }

    ACTION dohdocs::delauthor(const name author, const uint32_t faction_id, const uint32_t language_id, const name editor) {
        require_auth(editor);
        
        editors_table editors(get_self(), get_self().value);
        auto editor_itr = editors.find(editor.value);
        eosio::check(editor_itr != editors.end(), "Editor not registered");

        authors_table authors(get_self(), get_self().value);
        auto author_key = composite_key_128(author.value, faction_id, language_id);
        auto author_itr = authors.get(author_key, "Author not found");

        authors.erase(author_itr);
    }

    ACTION dohdocs::regeditor(const name editor, const uint32_t faction_id) {
        require_auth(get_self());
        
        editors_table editors(get_self(), get_self().value);
        editors.emplace(get_self(), [&](auto& row) {
            row.id = editor.value;
            row.account = editor;
            row.faction_id = faction_id;
        });
    }

    ACTION dohdocs::deleditor(const name editor, const uint32_t faction_id) {
        require_auth(get_self());
        
        editors_table editors(get_self(), get_self().value);
        auto editor_itr = editors.find(editor.value);
        eosio::check(editor_itr != editors.end(), "Editor not registered");

        editors.erase(editor_itr);
    }

    ACTION dohdocs::setcategory(const uint64_t category_id, const std::string name, const std::string description) {
        require_auth(get_self()); // Ensure that only the contract account can call this action.
        
        categories_table categories(get_self(), get_self().value); // Get the table for categories.
        
        auto cat_itr = categories.find(category_id); // Try to find the category by its ID.

        if (cat_itr == categories.end()) {
            // The category doesn't exist yet, so create a new one.
            categories.emplace(get_self(), [&](auto& row) {
                row.id = category_id;
                row.name = name;
                row.description = description;
            });
        } else {
            // The category exists. We will update it.
            categories.modify(cat_itr, get_self(), [&](auto& row) {
                row.name = name;
                row.description = description;
            });
        }
    }

    ACTION dohdocs::delcategory(const uint64_t category_id) {
        require_auth(get_self()); // Ensure that only the contract account can call this action.
        
        categories_table categories(get_self(), get_self().value); // Get the table for categories.
        
        auto cat_itr = categories.find(category_id); // Try to find the category by its ID.
        eosio::check(cat_itr != categories.end(), "Category not found"); // If the category doesn't exist, throw an error.

        categories.erase(cat_itr); // Remove the category from the table.
    }


}
