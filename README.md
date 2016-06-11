# node-xapian

For Node v0.10.x.

This is a forked version, just quickly hacked to work:

- The license is not here yet, but apparently it was a GSoC project,
  hence using some OSI approved license.

- `libmime2text.a` is removed, primarily because it's binary and
  caused build issues.

- Async tests are disabled, because they led to segfaults.

- Very basic `QueryParser` bindings are added.

- `package.json` is added.

- README is refined.


## Usage example

```
var xapian = require('xapian');
```

### Indexing

```
var xdb = new xapian.WritableDatabase(config.CouchDB.Names.Issues,
                                      xapian.DB_CREATE_OR_OPEN);
var doc = new xapian.Document();
var termgen = new xapian.TermGenerator();
doc.set_data(body.id);
termgen.set_document(doc);
termgen.index_text(params.description);
xdb.add_document(doc)
xdb.close();
```

### Searching

```
var xdb = new xapian.Database(config.CouchDB.Names.Issues);
var enq = new xapian.Enquire(xdb);
var queryParser = new xapian.QueryParser();
var query = queryParser.parse_query(params.q);
enq.set_query(query);
var results = enq.get_mset(0, 10);
var rs = results.map(function (x) {
  return {
      id: x.document.get_data()
    };
});
xdb.close();
```


## Old README

Mirrors Xapian API closely, except:
  Enquire::get_mset() returns an Array, not an iterator
  assemble_document() takes a document parameters object and returns a Document
  Mime2Text provides file-conversion logic from the omindex indexing utility

Classes
  Database
  WritableDatabase
  TermGenerator
  Stem
  Enquire
  Query
  Document
  Mime2Text
  
Documentation in docs.md

Mime2Text surfaces a proposed Xapian patch from
  https://github.com/networkimprov/xapian/commits/liam_mime2text-lib
  that patch is included here as an x86 Linux binary, libmime2text.a
