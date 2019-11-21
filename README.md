# Veronica
Multi-language frequently-used Library developed by Bowen

# Usage

`./setup.sh`
Or you can write the export cmd into your login profile, e.g. bashrc or zshrc.

# Perl
For using the perl modules in Veronica, exec `source /path_to_veronica/perl/setup.sh`
Then adding these lines to your perl codes (to use `Veronica::Common` as an example)

use lib "$ENV{'VERONICA'}/perl";

`use Veronica::Common;`
