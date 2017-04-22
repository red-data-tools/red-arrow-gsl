# Copyright 2017 Kouhei Sutou <kou@clear-code.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

require "arrow"
require "gsl"

require "arrow-gsl/version"

base_dir = Pathname.new(__dir__).dirname.expand_path
vendor_dir = base_dir + "vendor" + "local"

GLib.prepend_dll_path(vendor_dir + "bin")
begin
  major, minor, _ = RUBY_VERSION.split(/\./)
  require "#{major}.#{minor}/arrow_gsl.so"
rescue LoadError
  require "arrow_gsl.so"
end
