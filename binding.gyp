{
  'targets': [
    {
      'target_name': 'xapian-binding',
      'sources': [ "src/binding.cc", "src/database.cc", "src/document.cc", "src/enquire.cc", 
        "src/query.cc", "src/stem.cc", "src/rset.cc", "src/termgenerator.cc",
        "src/writabledatabase.cc", "src/queryparser.cc" ],
      'link_settings': {
        'libraries': [ '<!(xapian-config --libs)' ]
      },
      'cflags_cc': [ '<!(xapian-config --cxxflags)', "-g", "-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE", "-Wall"],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'conditions': [
		  ['OS=="mac"', {
			'xcode_settings': {
			  'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
			}
		  }]
      ]
    }
  ]
}
