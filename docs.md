Database
```
  methods similar to the C++ API
*    Database([function])
        Create a Database with no databases in. 
*    Database(string_path, [function])
        Open a Database, automatically determining the database backend to use. 
*    add_database(Database, [function])
        Add an existing database (or group of databases) to those accessed by this object. 
*    reopen([function])
        Re-open the database. 
*    close([function])
        Close the database.
*    get_description([function]) - return string
        Return a string describing this object. 
*    has_positions([function]) - return bool
        Does this database have any positional information? 
*    get_doccount([function]) - return uint32
        Get the number of documents in the database. 
*    get_lastdocid([function]) - return uint32
        Get the highest document id which has been used in the database. 
*    get_avlength([function]) - return number
        Get the average length of the documents in the database. 
*    get_termfreq(string_tname, [function]) - return uint32
        Get the number of documents in the database indexed by a given term. 
*    term_exists(string_tname, [function]) - return bool
        Check if a given term exists in the database. 
*    get_collection_freq(string_tname, [function]) - return uint32
        Return the total number of occurrences of the given term. 
*    get_value_freq(uint32_slot, [function]) - return uint32
        Return the frequency of a given value slot. 
*    get_value_lower_bound(uint32_slot, [function]) - return string
        Get a lower bound on the values stored in the given value slot. 
*    get_value_upper_bound_sync(uint32_slot, [function]) - return string
        Get an upper bound on the values stored in the given value slot. 
*    get_doclength_lower_bound([function]) - return uint32
        Get a lower bound on the length of a document in this DB. 
*    get_doclength_upper_bound([function]) - return uint32
        Get an upper bound on the length of a document in this DB. 
*    get_wdf_upper_bound(string_term, [function]) - return uint32
        Get an upper bound on the wdf of term term. 
*    get_doclength(uint32_did, [function]) - return uint32
        Get the length of a document. 
*    keep_alive([function])
        Send a "keep-alive" to remote databases to stop them timing out. 
*    get_document(uint32_did, [function]) - return object_document
        Get a document from the database, given its document id. 
*    get_spelling_suggestion(string_word, uint32_max_edit_distance=2, [function]) - return string
        Suggest a spelling correction.
*    get_metadata(string_key, [function]) - return string
        Get the user-specified metadata associated with a given key. 
*    get_uuid([function]) - return string
        Get a UUID for the database. 
  methods different from the C++ API:
    postlist_sync(string_tname) - return an array of { docid: uint32, doclength: uint32, wdf: uint32, description: string } (PostingIterator)
        An array of the postlists for a given term.
    termlist_sync(uint32_did) - return an array of { tname: string, wdf: uint32, termfreq: uint32, description: string } (TermIterator)
        An array of the  terms for a given document. 
    positionlist_sync(uint32_did, string_tname) - return an array of { description: string } (PositionIterator)
        An array of the positions for a given term in a given document.
    allterms_sync() - return an array of { tname: string, wdf: uint32, termfreq: uint32, description: string } (TermIterator)
        An array of all the terms in the database. 
    allterms_sync(string_prefix) - return an array of { tname: string, wdf: uint32, termfreq: uint32, description: string } (TermIterator)
        An array of all the terms with a given prefix in the database. 
    valuestream_sync(uint32_slot) - return an array of { value: string, docid: uint32, valueno: uint32, description: string } (ValueIterator)
        An array of the values in slot slot for each document. 
    spellings_sync() - return an array of { tname: string, wdf: uint32, termfreq: uint32, description: string } (TermIterator)
        An array of all the spelling correction targets. 
    synonyms_sync() - return an array of { tname: string, wdf: uint32, termfreq: uint32, description: string } (TermIterator)
        An array of all the synonyms for a given term. 
    synonym_keys_sync(string_prefix='') - return an array of { tname: string, wdf: uint32, termfreq: uint32, description: string } (TermIterator)
        An array of all terms which have synonyms. 
    metadata_keys_sync(string_prefix='') - return an array of { tname: string, wdf: uint32, termfreq: uint32, description: string } (TermIterator)
        An array of all user-specified metadata keys. 

```
WritableDatabase - all the methods from Database plus the following
```
  methods similar to the C++ API
*    WritableDatabase([function])
        Create an empty WritableDatabase.
*    WritableDatabase(string_path, uint32_action, [function])
        Open a database for update, automatically determining the database backend to use.
          uint32_action can be: DB_CREATE_OR_OPEN, DB_CREATE, DB_CREATE_OR_OVERWRITE, DB_OPEN
*    commit([function])
        Commit any pending modifications made to the database.  
*    begin_transaction([bool_flushed=true], [function])
        Begin a transaction. 
*    commit_transaction([function])
        Complete the transaction currently in progress. 
*    cancel_transaction_sync()
        Abort the transaction currently in progress, discarding the pending modifications made to the database. 
*    add_document(object_document, [function]) - return uint32
        Add a new document to the database. 
    delete_document_sync(uint32_did)
        Delete a document from the database. 
    delete_document_sync(string_unique_term)
        Delete any documents indexed by a term from the database. 
*    replace_document(uint32_did, object_document, [function])
        Replace a given document in the database. 
*    replace_document(string_unique_term, object_document, [function]) - return uint32
        Replace any documents matching a term.
*    replace_document(null, object_document, [function]) - return uint32
        The method acts as add_document.
    add_spelling_sync(string_word, uint32_freqinc=1)
        Add a word to the spelling dictionary. 
    remove_spelling_sync(string_word, uint32_freqdec=1)
        Remove a word from the spelling dictionary. 
    add_synonym_sync(string_term, string_synonym)
        Add a synonym for a term. 
    remove_synonym_sync(string_erm, string_synonym)
        Remove a synonym for a term. 
    clear_synonyms_sync(string_term)
        Remove all synonyms for a term. 
    set_metadata_sync(string_key, string_value)
        Set the user-specified metadata associated with a given key. 
    get_description_sync() - return string
        Return a string describing this object.
```
Document
```
  methods similar to the C++ API
*    Document()
        Make a new empty Document. 
*    get_value(uint32_slot, [function]) - return string
        Get value by number.
*    add_value(uint32_slot, string_value, [function])
        Add a new value. 
*    remove_value(uint32_slot, [function])
        Remove any value with the given number. 
*    clear_values([function])
        Remove all values associated with the document. 
*    get_data([function]) - return string
        Get data stored in the document. 
*    set_data(string_data, [function])
        Set data stored in the document. 
*    add_posting(string_tname, uint32_tpos, [uint32_wdfinc=1], [function])
        Add an occurrence of a term at a particular position. 
*    add_term(string_tname, [uint32_wdfinc=1], [function])
        Add a term to the document, without positional information. 
*    add_boolean_term(string_term, [function])
        Add a boolean filter term to the document. 
*    remove_posting(string_tname, uint32_tpos, [uint32_wdfdec=1], [function])
        Remove a posting of a term from the document. 
*    remove_term(string_tname, [function])
        Remove a term and all postings associated with it. 
*    clear_terms([function])
        Remove all terms (and postings) from the document. 
*    termlist_count([function]) - return uint32
        The length of the termlist - i.e. 
*    values_count([function]) - return uint32
        Count the values in this document. 
*    get_docid([function]) - return uint32
        Get the document id which is associated with this document (if any). 
*    serialise([function]) - return string
        Serialise document into a string. 
*    get_description([function]) - return string
        Return a string describing this object. 
*    unserialise(string_s, [function]) - return object_document
        Unserialise a document from a string produced by serialise().
  methods different from the C++ API:
*    termlist([uint32_first], [uint32_maxitems], [function]) - return an array of { tname: string, wdf: uint32, description: string } (TermIterator)
        An array of the terms in this document. 
*    values([uint32_first], [uint32_maxitems], [function]) - return an array of { value: string, valueno: uint32, description: string } (ValueIterator)
        An array of the values in this document. 
```
Enquire
```
  methods similar to the C++ API
*    Enquire(object_database)
        Create a Xapian::Enquire object. 
    set_query_sync(object_query, uint32_qlen=0)
        Set the query to run. 
    get_query_sync() - return object_query
        Get the query which has been set. 
    set_parameters_sync( object_parameters)
        Set the parameters to be used for queries.
        The object parameter can have one or more of the following:
          {
            collapse_key: { key: uint32, max: uint32=1},
          	docid_order: uint32,
          	cutoff: { percent: int32, weight: number=0 },
          	sort: [ sort_by_info_1, ... ]
          }
        The sort_by_info object can be:
        	RELEVANCE - sorting by relevance
        	{ key: string, reverse: bool } - sorting by value (with reverse)
        	string_value_key - sorting by value
        The valid sort arrays currently are:
        	[ RELEVANCE ] - sort_by_relevance
        	[ { key: string_value_key, reverse: bool } ] - sort_by_value
        	[ string_value_key ] - sort_by_value
        	[ { key: string_value_key, reverse: bool }, RELEVANCE ] - sort_by_value_then_relevance
        	[ string_value_key, RELEVANCE ] - sort_by_value_then_relevance
        	[ RELEVANCE, { key: string_value_key, reverse: bool } ] - sort_by_relevance_then_value
        	[ RELEVANCE, string_value_key ] - sort_by_relevance_then_value  
    get_description_sync() - return string
        Return a string describing this object.     
  methods different from the C++ API:
    get_eset_sync (uint32_maxitems, object_omrset, int32_flags=0, number_k=1.0) - return an array of { term: string, weight: number, description: string } (ESet)
        Get the expand set for the given rset. 
        flags: TODO
    get_matching_terms_sync (uint32_did) - return an array of { tname: string, wdf: uint32, termfreq: uint32, description: string } (TermIterator)
        Get terms which match a given document, by document id. 
    get_mset(uint32_first, uint32_maxitems, uint32_checkatleast=0, object_omrset=null) - return an array of { document: object_document, id: uint32, rank: uint32,  collapse_count: uint32, weight: number, collapse_key: string, description: string, percent: int32 } (MSet)
        Get (a portion of) the match set for the current query.
```
Query
```
  methods similar to the C++ API
    Query ()
        Default constructor: makes an empty query which matches no documents. 
*    Query( object_query_structure )
        A query defined by a query structure.
          A query structure can be defined as:
            string - http://xapian.org/docs/apidoc/html/classXapian_1_1Query.html#f396e213df0d8bcffa473a75ebf228d6
            {tname: string, wqf: uint32=1, pos: uint32=0} - http://xapian.org/docs/apidoc/html/classXapian_1_1Query.html#f396e213df0d8bcffa473a75ebf228d6
            {op: string, queries: [QueryObject, ...], parameter: number=0} - http://xapian.org/docs/apidoc/html/classXapian_1_1Query.html#a2edfc5e6ae285afb9fde26d096c5ed8
            {op: string, left: string, right: string} - http://xapian.org/docs/apidoc/html/classXapian_1_1Query.html#cd51f1304501311c8dde31dc54c36048
            {op: string, query: QueryObject, parameter: number} - http://xapian.org/docs/apidoc/html/classXapian_1_1Query.html#4004922547fec06087dc8a63ac484f9b
            {op: string, slot: uint32, begin: string, end: string} - http://xapian.org/docs/apidoc/html/classXapian_1_1Query.html#27acaeb8ab8a0c111805a0139e3dc35b
            {op: string, slot: uint32, value: string} - http://xapian.org/docs/apidoc/html/classXapian_1_1Query.html#15e26a7bf7de2699377cad9e9448a482
    get_length_sync() - return uint32
        Get the length of the query, used by some ranking formulae. 
    get_terms_sync() - return an array of { tname: string, wdf: uint32, termfreq: uint32, description: string } (TermIterator)
        An array of all the terms in the query, in order of termpos. 
    empty_sync() - return bool
        Test if the query is empty (i.e. 
    serialise_sync() - return string
        Serialise query into a string. 
    get_description_sync() - return string
        Return a string describing this object. 
    unserialise_sync(string) - return object_query
        Unserialise a query from a string produced by serialise(). 
    MatchAll() - return object_query
        A query which matches all documents in the database. 
    MatchNothing() - return object_query
        A query which matches no documents.
```
RSet
```
  methods similar to the C++ API
    RSet()
        Default constructor. 
    size_sync() - return uint32
        The number of documents in this R-Set. 
    empty_sync() - return bool
        Test if this R-Set is empty. 
    add_document_sync(uint32_did)
        Add a document to the relevance set. 
    remove_document_sync(uint32_did)
        Remove a document from the relevance set. 
    contains_sync(uint32_did) - return bool
        Test if a given document in the relevance set. 
    get_description_sync() - return string
        Return a string describing this object.
```
Stem
```
  methods similar to the C++ API
    Stem()
        Construct a Xapian::Stem object which doesn't change terms. 
    Stem(string_language)
        Construct a Xapian::Stem object for a particular language. 
    stem_word_sync(string_word) - return string
        Stem a word. 
    get_description_sync() - return string
        Return a string describing this object. 
    get_available_languages() - return string
        Return a list of available languages.
```
TermGenerator
```
  methods similar to the C++ API
    TermGenerator()
        Default constructor. 
    set_stemmer_sync(object_stemmer)
        Set the Xapian::Stem object to be used for generating stemmed terms. 
    set_document_sync(object_document)
        Set the current document. 
    get_document_sync() - return object_document
        Get the current document. 
    set_database_sync(object_writableDatabase)
        Set the database to index spelling data to. 
    set_flags_sync(int32_flags)
        Set flags. 
    index_text_sync(string_text, uint32_wdf_inc=1, string_prefix="")
        Index some text in a std::string. 
    index_text_without_positions_sync(string_text, uint32_wdf_inc=1, string_prefix="")
        Index some text in a std::string without positional information. 
    increase_termpos_sync(uint32_delta=100)
        Increase the term position used by index_text. 
    get_termpos_sync() - return uint32
        Get the current term position. 
    set_termpos_sync(uint32_termpos)
        Set the current term position. 
    get_description_sync() - return string
        Return a string describing this object.
```






