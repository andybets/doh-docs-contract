#pragma once

#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <string>

namespace eosio {

   using std::string;

   class [[eosio::contract("dohdocs")]] dohdocs : public contract {
      public:
         using contract::contract;

         // adds a candidate tooltip document for the faction and language specified
         // the author must be a registered author for the faction and language specified
         // author auth required, and author pays for multiindex ram
         ACTION adddoc(const uint64_t item_id, const uint32_t faction_id, const uint32_t language_id, const uint64_t category_id, const name author, const std::string title, const std::string content);

         // deletes a candidate tooltip document for the faction and language specified
         // only the author of the document, or the editor for the faction/language may delete it
         ACTION deldoc(const uint64_t item_id, const uint32_t faction_id, const uint32_t language_id, const name author);

         // publishes a candidate tooltip document for the faction and language specified, and copies it to the published table
         // the editor must be a registered editor for the faction specified
         // editor pays for multiindex ram
         ACTION publish(const uint64_t item_id, const uint32_t faction_id, const uint32_t language_id, const name editor);

         // unpublishes a candidate tooltip document for the faction and language specified, and removes it from the published table
         ACTION unpublish(const uint64_t item_id, const uint32_t faction_id, const uint32_t language_id, const name editor);

         // registers an author who can submit any document for the faction and language specified
         // an author may be registered several times for different factions/languages
         // the editor must be a registered editor for the faction specified
         // editor pays for multiindex ram
         ACTION regauthor(const name author, const uint32_t faction_id, const uint32_t language_id, const name editor);

         // deletes an author record for the faction and language specified
         // the editor must be a registered editor for the faction specified
         ACTION delauthor(const name author, const uint32_t faction_id, const uint32_t language_id, const name editor);

         // registers an editor for the factions specified
         // requires contract authorisation
         ACTION regeditor(const name editor, const uint32_t faction_id);

         // unregisters an editor for the factions specified
         // requires contract authorisation
         ACTION deleditor(const name editor, const uint32_t faction_id);

         // adds item category
         // requires contract authorisation
         ACTION setcategory(const uint64_t category_id, const std::string name, const std::string description);

         // deleted category
         // requires contract authorisation
         ACTION delcategory(const uint64_t category_id);

         static uint128_t composite_key_128(const uint64_t &item_id, const uint32_t &faction_id, const uint32_t &language_id) {
             return (uint128_t{item_id} << 64) | (uint128_t{faction_id} << 32) | language_id;
         }

      private:

         /* stores documents which have been submitted, but not yet approved */
         TABLE candidatedoc {
            uint64_t      id;
            uint64_t      item_id;
            uint32_t      faction_id;
            uint32_t      language_id;
            uint64_t      category_id;
            name          author;
            std::string   title;
            std::string   content;
            uint64_t primary_key() const { return id; }
            uint128_t by_composite_key() const { return composite_key_128(item_id, faction_id, language_id); }
         };

         /* stores documents which have been approved by the relevant editor for the faction */
         TABLE  publisheddoc {
            uint64_t      id;
            uint64_t      item_id;
            uint32_t      faction_id;
            uint32_t      language_id;
            uint64_t      category_id;
            name          author;
            std::string   title;
            std::string   content;
            time_point    approved_at;
            name          approved_by;
            uint64_t primary_key() const { return id; }
            uint128_t by_composite_key() const { return composite_key_128(item_id, faction_id, language_id); }
            uint64_t by_category() const { return category_id; }
         };

         /* stores the list of item categories */
         TABLE category {
            uint64_t      id;
            std::string   name;
            std::string   description;
            uint64_t primary_key() const { return id; }
         };

         /* stores the list of authors permitted to submit documents */
         TABLE author {
            uint64_t      id;
            name          account;
            uint32_t      faction_id;
            uint32_t      language_id;
            uint64_t primary_key() const { return id; }
            uint128_t by_composite_key() const { return composite_key_128(account.value, faction_id, language_id); }
         };

         /* stores the list of editors permitted to publish documents */
         TABLE editor {
            uint64_t      id;
            name          account;
            uint32_t      faction_id;
            uint64_t primary_key() const { return id; }
            uint64_t by_editor() const { return account.value; }
         };

         typedef eosio::multi_index< "candidates"_n, candidatedoc,
            indexed_by<"composite"_n, const_mem_fun<candidatedoc, uint128_t, &candidatedoc::by_composite_key> > > candidatedocs_table;

         typedef eosio::multi_index< "published"_n, publisheddoc,
             indexed_by<"composite"_n, const_mem_fun<publisheddoc, uint128_t, &publisheddoc::by_composite_key>>,
             indexed_by<"bycategory"_n, const_mem_fun<publisheddoc, uint64_t, &publisheddoc::by_category>> > publisheddocs_table;

         typedef eosio::multi_index< "categories"_n, category > categories_table;

         typedef eosio::multi_index< "authors"_n, author,
            indexed_by<"composite"_n, const_mem_fun<author, uint128_t, &author::by_composite_key> > > authors_table;

         typedef eosio::multi_index< "editors"_n, editor,
            indexed_by<"editor"_n, const_mem_fun<editor, uint64_t, &editor::by_editor> > > editors_table;

   };
}
