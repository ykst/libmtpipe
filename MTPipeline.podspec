Pod::Spec.new do |s|
  s.name         = "MTPipeline"
  s.version      = "0.2.0"
  s.summary      = "low-level multithread pipeline library."

  s.description  = <<-DESC
                   DESC

  s.homepage     = "https://github.com/ykst/libmtpipe"
  s.license      = { :type => 'MIT', :file => 'MIT-LICENSE.txt' }

  s.author       = { "Yohsuke Yukishita" => "ykstyhsk@gmail.com" }

  s.source       = { :git => "https://github.com/ykst/libmtpipe.git", :tag => "0.2.0" }

  #s.header_mappings_dir = 'external/cinclude/include'
  s.source_files  = 'src/*.{h,c,m}', 'include/*.h', 'external/cinclude/**/*.h'
  s.exclude_files = 'Makefile'
  s.prefix_header_file = 'external/cinclude/include/common.h'
  s.public_header_files = 'include/*.h'
  s.requires_arc = true

end
