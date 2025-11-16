# Texinfo.pm: format Pod as Texinfo.
#
# Copyright 2011, 2012, 2013, 2014, 2022 Free Software Foundation, Inc.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License,
# or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# 
# Original author: Patrice Dumas <pertusus@free.fr>
# Parts from L<Pod::Simple::HTML>.
#
#
# The code could easily be used directly as a Pod::Simple subclass
# by renaming _texinfo_handle_element_start and the two other
# similar functions as _handle_element_start, or as a
# Pod::Simple::SimpleTree subclass, using _convert_pod_simple_tree.
# We prefer a Pod::Simple::PullParser subclassing to be able to use
# get_short_title().
#
#
# TODO: it could be relevant to convert L<...> referring to external modules to
# urls to Metacpan, or similar websites, using @url instead of @ref, which
# is very unlikely to lead to other modules documentation.


package Pod::Simple::Texinfo;

require 5;
use strict;
#no autovivification qw(fetch delete exists store strict);

use Carp qw(cluck);
#use Pod::Simple::Debug (3);
use Pod::Simple::PullParser ();

# for parselink()
#use Pod::ParseLink;

use Texinfo::Convert::NodeNameNormalization qw(normalize_node);
use Texinfo::Parser qw(parse_texi_line parse_texi_text);
use Texinfo::Convert::Texinfo;
use Texinfo::Convert::TextContent;
use Texinfo::Common qw(protect_colon_in_tree protect_comma_in_tree
                                         protect_first_parenthesis);
use Texinfo::Transformations qw(protect_hashchar_at_line_beginning
                                          reference_to_arg_in_tree);

use vars qw(
  @ISA $VERSION
);

@ISA = ('Pod::Simple::PullParser');
$VERSION = '0.01';

#use UNIVERSAL ();

# Allows being called from the command line as
# perl -w -MPod::Simple::Texinfo -e Pod::Simple::Texinfo::go thingy.pod
sub go { Pod::Simple::Texinfo->parse_from_file(@ARGV); exit 0 }

my %standard_headers;
# from the main list of perlpodstyle
foreach my $standard_header ('NAME', 'SYNOPSIS', 'DESCRIPTION', 'OPTIONS',
 'RETURN VALUE', 'ERRORS', 'DIAGNOSTICS', 'EXAMPLES', 'ENVIRONMENT',
 'FILES', 'CAVEATS', 'WARNINGS', 'BUGS', 'RESTRICTIONS', 'NOTES', 'AUTHOR',
 'AUTHORS', 'HISTORY', 'COPYRIGHT AND LICENSE', 'SEE ALSO',
# additional text after the list.
# 'CONFORMING TO' and 'MT-LEVEL' ignored as they seem to be very specific.
 'CONSTRUCTORS', 'METHODS', 'CLASS METHODS', 'INSTANCE METHODS', 'FUNCTIONS',
 'OVERVIEW',
# from perldocstyle
 'CONTRIBUTORS', 'COPYRIGHT', 'LICENSE') {
  $standard_headers{lc($standard_header)} = 1;
}

my %pod_head_commands_level;
foreach my $level (1 .. 4) {
  $pod_head_commands_level{'head'.$level} = $level;
}

my @numbered_sectioning_commands = ('part', 'chapter', 'section', 'subsection',
  'subsubsection');
my @appendix_sectioning_commands = ('part', 'appendix', 'appendixsec',
  'appendixsubsec', 'appendixsubsubsec');
my @unnumbered_sectioning_commands = ('part', 'unnumbered', 'unnumberedsec',
  'unnumberedsubsec', 'unnumberedsubsubsec');
my @heading_commands = ('part', 'chapheading', 'heading', 'subheading',
   'subsubheading');

my @raw_formats = ('html', 'HTML', 'docbook', 'DocBook', 'texinfo',
                       'Texinfo');

# from other Pod::Simple modules.  Creates accessor subroutine.
__PACKAGE__->_accessorize(
  'texinfo_add_upper_sectioning_command',
  'texinfo_internal_pod_manuals',
  'texinfo_man_url_prefix',
  'texinfo_main_command_sectioning_style',
  'texinfo_section_nodes',
  'texinfo_sectioning_base_level',
  'texinfo_sectioning_style',
  'texinfo_short_title',
);

my $sectioning_style = 'numbered';
#my $sectioning_base_level = 2;
my $sectioning_base_level = 0;
my $man_url_prefix = 'http://man.he.net/man';

sub new
{
  my $class = shift;
  my $new = $class->SUPER::new(@_);
  $new->accept_targets(@raw_formats);
  $new->preserve_whitespace(1);
  $new->texinfo_section_nodes(0);
  $new->texinfo_sectioning_base_level($sectioning_base_level);
  $new->texinfo_man_url_prefix($man_url_prefix);
  $new->texinfo_sectioning_style($sectioning_style);
  $new->texinfo_add_upper_sectioning_command(1);
  #$new->{'texinfo_nodes'} = {};
  return $new;
}

sub run
{
  my $self = shift;

  # In case the caller changed the formats
  my @formats = $self->accept_targets();
  $self->{'texinfo_raw_format_commands'} = {};
  $self->{'texinfo_if_format_commands'} = {};
  foreach my $format (@formats) {
    if (lc($format) eq 'texinfo') {
      $self->{'texinfo_raw_format_commands'}->{$format} = '';
      $self->{'texinfo_if_format_commands'}->{':'.$format} = '';
    } else {
      $self->{'texinfo_raw_format_commands'}->{$format} = lc($format);
      $self->{'texinfo_if_format_commands'}->{':'.$format} = lc($format);
    }
  }
  my $base_level = $self->texinfo_sectioning_base_level();
  $base_level = 1 if ($base_level <= 1);
  my $main_command_sectioning_style
    = $self->texinfo_main_command_sectioning_style();
  if ($self->texinfo_sectioning_style() eq 'numbered') {
    $self->{'texinfo_sectioning_commands'} = \@numbered_sectioning_commands;
  } elsif ($self->texinfo_sectioning_style() eq 'unnumbered') {
    $self->{'texinfo_sectioning_commands'} = \@unnumbered_sectioning_commands;
  } elsif ($self->texinfo_sectioning_style() eq 'heading') {
    $self->{'texinfo_sectioning_commands'} = \@heading_commands;
    if (! defined($main_command_sectioning_style)) {
      $main_command_sectioning_style = 'numbered';
    }
  } else {
    $self->{'texinfo_sectioning_commands'} = \@appendix_sectioning_commands;
  }
  $main_command_sectioning_style = $self->texinfo_sectioning_style()
    if (not defined($main_command_sectioning_style));
  if ($main_command_sectioning_style eq 'numbered') {
    $self->{'texinfo_sectioning_main_command'} = \@numbered_sectioning_commands;
  } elsif ($main_command_sectioning_style eq 'unnumbered') {
    $self->{'texinfo_sectioning_main_command'} = \@unnumbered_sectioning_commands;
  } else {
    $self->{'texinfo_sectioning_main_command'} = \@appendix_sectioning_commands;
  }

  $self->{'texinfo_head_commands'} = {};
  foreach my $heading_command (keys(%pod_head_commands_level)) {
    my $level = $pod_head_commands_level{$heading_command} + $base_level -1;
    if (defined($self->{'texinfo_sectioning_commands'}->[$level])) {
      $self->{'texinfo_head_commands'}->{$heading_command}
        = $self->{'texinfo_sectioning_commands'}->[$level];
    } else {
      $self->{'texinfo_head_commands'}->{$heading_command}
        = $self->{'texinfo_sectioning_commands'}->[-1];
    }
  }
  # contain all the manuals that are part of the same output
  $self->{'texinfo_internal_pod_manuals_hash'} = {};
  my $manuals = $self->texinfo_internal_pod_manuals();
  if ($manuals) {
    foreach my $manual (@$manuals) {
      $self->{'texinfo_internal_pod_manuals_hash'}->{$manual} = 1;
    }
  }

  $self->{'texinfo_nodes'} = {};

  if ($self->bare_output()) {
    $self->_convert_pod_tokens();
  } else {
    $self->_preamble();
    $self->_convert_pod_tokens();
    $self->_postamble();
  }
}

my $STDIN_DOCU_NAME = 'stdin';
sub _preamble($)
{
  my $self = shift;

  $self->{'texinfo_accumulated'} = [];
  $self->{'texinfo_stack'} = [];

  my $fh = $self->{'output_fh'};

  if (!defined($self->texinfo_short_title())) {
    # from Pod::Simple::PullParser
    my $short_title = $self->get_short_title();
    if (defined($short_title) and $short_title =~ m/\S/) {
      $self->texinfo_short_title($short_title);
    }
  }

  if ($self->texinfo_sectioning_base_level() == 0) {
    #print STDERR "$fh\n";
    print $fh '\input texinfo'."\n";
    my $setfilename;
    if (defined($self->texinfo_short_title())) {
      $setfilename = _pod_title_to_file_name($self->texinfo_short_title());
    } else {
      # FIXME maybe output filename would be better than source_filename?
      my $source_filename = $self->source_filename();
      if (defined($source_filename) and $source_filename ne '') {
        if ($source_filename eq '-') {
          $setfilename = $STDIN_DOCU_NAME;
        } else {
          $setfilename = $source_filename;
          $setfilename =~ s/\.(pod|pm)$//i;
        }
      }
    }
    if (defined($setfilename) and $setfilename =~ m/\S/) {
      $setfilename = _protect_text($setfilename, 1, 1);
      $setfilename .= '.info';
      print $fh "\@setfilename $setfilename\n\n"
    }

    my $title = $self->get_title();
    if (defined($title) and $title =~ m/\S/) {
      print $fh "\@settitle "._protect_text($title, 1)."\n\n";
    }
    print $fh "\@node Top\n";
    if (defined($self->texinfo_short_title())) {
      print $fh "\@top "._protect_text($self->texinfo_short_title(), 1)."\n\n";
    }
  } elsif (defined($self->texinfo_short_title())
           and $self->texinfo_add_upper_sectioning_command()) {
    my $level = $self->texinfo_sectioning_base_level() - 1;
    my $name = _protect_text($self->texinfo_short_title(), 1, 1);
    my $node_name = _prepare_anchor($self, $name);

    my $anchor = '';
    my $node = '';
    if ($node_name =~ /\S/) {
      if (!$self->texinfo_section_nodes()
          or $self->{'texinfo_sectioning_main_command'}->[$level] eq 'part') {
        $anchor = "\@anchor{$node_name}\n";
      } else {
        $node = "\@node $node_name\n";
      }
    }
    print $fh "$node\@$self->{'texinfo_sectioning_main_command'}->[$level] "
       ._protect_text($self->texinfo_short_title(), 1)."\n$anchor\n";
  }
}

# 'out' is out of the context, for now for index entries.
sub _output($$$;$)
{
  my $fh = shift;
  my $accumulated_stack = shift;
  my $text = shift;
  my $out = shift;

  if (scalar(@$accumulated_stack)) {
    if ($out) {
      $accumulated_stack->[-1]->{'out'} .= $text;
    } else {
      $accumulated_stack->[-1]->{'text'} .= $text;
    }
  } else {
    print $fh $text;
  }
}

sub _begin_context($$)
{
  my $accumulated_stack = shift;
  my $tag = shift;
  push @$accumulated_stack, {'text' => '', 'tag' => $tag,
                             'out' => ''};
}

sub _end_context($)
{
  my $accumulated_stack = shift;
  my $previous_context = pop @$accumulated_stack;
  return ($previous_context->{'text'}, $previous_context->{'out'});
}

sub _protect_text($;$$)
{
  my $text = shift;
  my $remove_new_lines = shift;
  my $in_code = shift;
  cluck if (!defined($text));
  $text =~ s/\n/ /g if ($remove_new_lines);
  $text =~ s/([\@\{\}])/\@$1/g;
  # from perlpodspec
  # Pod parsers should not, by default, try to coerce apostrophe (') and quote
  # (") into smart quotes (little 9's, 66's, 99's, etc), nor try to turn
  # backtick (`) into anything else but a single backtick character (distinct
  # from an open quote character!), nor "--" into anything but two minus signs.
  # They must never do any of those things to text in C<...> formatting codes,
  # and never ever to text in verbatim paragraphs.
  #
  # In Texinfo, -- --- would become dashes if not in verbatim/code.  We make
  # sure that it is not the case here.  We do not do the same for the backticks
  # and apostrophes, as there is no way to prevent those from becoming smart
  # quotes without putting them in @code{} or similar @-command.
  if (! $in_code) {
    $text =~ s/---/\@asis{}-\@asis{}-\@asis{}-\@asis{}/g;
    $text =~ s/--/\@asis{}-\@asis{}-\@asis{}/g
  }
  return $text;
}

sub _pod_title_to_file_name($)
{
  my $name = shift;
  $name =~ s/\s+/_/g;
  $name =~ s/::/-/g;
  $name =~ s/[^\w\.-]//g;
  $name = '_' if ($name eq '');
  return $name;
}

sub _protect_comma($)
{
  my $texinfo = shift;
  my $tree = parse_texi_line(undef, $texinfo);
  $tree = protect_comma_in_tree($tree);
  return Texinfo::Convert::Texinfo::convert_to_texinfo($tree);
}

sub _protect_colon($)
{
  my $texinfo = shift;
  my $tree = parse_texi_line(undef, $texinfo);
  $tree = protect_colon_in_tree($tree);
  return Texinfo::Convert::Texinfo::convert_to_texinfo($tree);
}

sub _protect_hashchar($)
{
  my $texinfo = shift;
  # protect # first in line
  if ($texinfo =~ /#/) {
    my $tree = parse_texi_text(undef, $texinfo);
    protect_hashchar_at_line_beginning(undef, undef, $tree);
    return Texinfo::Convert::Texinfo::convert_to_texinfo($tree);
  } else {
    return $texinfo;
  }
}

sub _reference_to_text_in_texi($)
{
  my $texinfo = shift;
  my $tree = parse_texi_text(undef, $texinfo);
  reference_to_arg_in_tree($tree);
  return Texinfo::Convert::Texinfo::convert_to_texinfo($tree);
}

sub _prepend_internal_section_manual($$$;$$)
{
  my $manual = shift;
  my $section = shift;
  my $base_level = shift;
  my $protect_text = shift;
  my $in_code = shift;

  if (defined($manual) and $base_level > 0) {
    $manual = _protect_text($manual, 1, $in_code) if ($protect_text);
    return "$manual $section";
  } else {
    return $section;
  }
}

sub _normalize_texinfo_name($$)
{
  # Pod may be more forgiven than Texinfo, so we go through
  # a normalization, by parsing and converting back to Texinfo
  my $name = shift;
  my $command = shift;
  my $texinfo_text;
  if ($command eq 'anchor') {
    $texinfo_text = "\@anchor{$name}";
  } else {
    # item is not correct since it cannot happen outside of a table
    # context, so we use @center which accepts the same on the line
    if ($command eq 'item') {
      $command = 'center';
    }
    $texinfo_text = "\@$command $name\n";
  }
  my $parser = Texinfo::Parser::parser();
  my $tree = $parser->parse_texi_piece($texinfo_text);
  if (!defined($tree)) {
    my $texinfo_text_str = $texinfo_text;
    chomp($texinfo_text_str);
    warn "ERROR: Texinfo parsing failed for: $texinfo_text_str\n";
    my $registrar = $parser->registered_errors();
    my ($parser_errors, $parser_error_count) = $registrar->errors();
    foreach my $error_message (@$parser_errors) {
      if ($error_message->{'type'} eq 'error') {
        warn "ERROR: $error_message->{'error_line'}";
      } else {
        warn "WARNING: $error_message->{'error_line'}";
      }
    }
    # FIXME Or undef, and callers check the return to be defined?
    return '';
  }
  if ($command eq 'anchor') {
    # FIXME this works to find the anchor command only if
    # the tree structure always leads to the interesting
    # elements being first in the contents.
    my $current = $tree;
    while ((not exists($current->{'cmdname'})
            or $current->{'cmdname'} ne 'anchor')
           and $current->{'contents'}
           and scalar(@{$current->{'contents'}})) {
      $current = $current->{'contents'}->[0];
    }
    if (not exists($current->{'cmdname'}) or $current->{'cmdname'} ne 'anchor') {
      cluck "BUG: could not find anchor: $texinfo_text";
    } elsif ($current->{'args'}->[0]->{'contents'}) {
      my $protected_contents
          = protect_first_parenthesis($current->{'args'}->[0]->{'contents'});
      $current->{'args'}->[0]->{'contents'} = $protected_contents;
    }
  }
  my $fixed_text = Texinfo::Convert::Texinfo::convert_to_texinfo($tree, 1);
  my $result = $fixed_text;
  if ($command eq 'anchor') {
    $result =~ s/^\@anchor\{(.*)\}$/$1/s;
  } else {
    chomp($result);
    $result =~ s/^\@$command (.*)$/$1/s;
  }
  return $result;
}

sub _node_name($$)
{
  my $self = shift;
  my $texinfo_node_name = shift;

  chomp $texinfo_node_name;
  $texinfo_node_name
   = _prepend_internal_section_manual($self->texinfo_short_title(),
                                  $texinfo_node_name,
                                  $self->texinfo_sectioning_base_level(), 1, 1);
  # also change refs to text
  return _reference_to_text_in_texi($texinfo_node_name);
}

sub _prepare_anchor($$)
{
  my $self = shift;
  my $texinfo_node_name = shift;

  my $node = _normalize_texinfo_name($texinfo_node_name, 'anchor');

  if ($node !~ /\S/) {
    return '';
  }
  # Now we know that we have something.
  my $node_tree = parse_texi_line(undef, $node);
  my $normalized_base = normalize_node($node_tree);
  my $normalized = $normalized_base;
  my $number_appended = 0;
  while ($self->{'texinfo_nodes'}->{$normalized}) {
    $number_appended++;
    $normalized = "${normalized_base}-$number_appended";
  }
  my $node_name;
  if ($number_appended) {
    $texinfo_node_name = "$node $number_appended";
    $node_tree = parse_texi_line(undef, $texinfo_node_name);
  }
  $node_tree = protect_comma_in_tree($node_tree);
  $node_tree = protect_colon_in_tree($node_tree);
  $self->{'texinfo_nodes'}->{$normalized} = $node_tree;
  my $final_node_name = Texinfo::Convert::Texinfo::convert_to_texinfo($node_tree, 1);
  return $final_node_name;
}

# from Pod::Simple::HTML general_url_escape
sub _url_escape($)
{
  my $string = shift;

  $string =~ s/([^\x00-\xFF])/join '', map sprintf('%%%02X',$_), unpack 'C*', $1/eg;
     # express Unicode things as urlencode(utf(orig)).

  # A pretty conservative escaping, behoovey even for query components
  #  of a URL (see RFC 2396)

  $string =~ s/([^-_\.!~*()abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789])/sprintf('%%%02X',ord($1))/eg;
   # Yes, stipulate the list without a range, so that this can work right on
   #  all charsets that this module happens to run under.
   # Altho, hmm, what about that ord?  Presumably that won't work right
   #  under non-ASCII charsets.  Something should be done
   #  about that, I guess?

  return $string;
}

my %tag_commands = (
  'F' => 'file',
  'S' => 'w',
  'I' => 'emph',
  'B' => 'strong', # or @b?
  'C' => 'code'
);

my %environment_commands = (
  'Verbatim' => 'verbatim',
  'over-text' => 'table @asis',
  'over-bullet' => 'itemize',
  'over-number' => 'enumerate',
  'over-block' => 'quotation',
);

my %line_commands = (
  'item-bullet' => 'item',
  'item-text' => 'item',
  'item-number' => 'item',
# =encoding is not emitted by Pod::Simple.  We get decoded characters here and
# assume that the callers always output utf8.
# It is ok, as it is right to use utf8 as encoding, and it matches the default
# of Texinfo, removing the need to set @documentencoding.  Even if =encoding was
# emitted, it would still be best to avoid @documentencoding and assume utf8 is
# output.
#  'encoding' => 'documentencoding'
);

foreach my $pod_head_command (keys(%pod_head_commands_level)) {
  $line_commands{$pod_head_command} = 1;
}

my %context_tags;
foreach my $context_tag (keys(%line_commands), 'L', 'X', 'Para') {
  $context_tags{$context_tag} = 1;
}

sub _texinfo_handle_element_start($$$)
{
  my $self = shift;
  my $tagname = shift;
  my $attr_hash = shift;

  my $fh = $self->{'output_fh'};

  # unset ignoring spaces right after <X> if there is a following tag,
  # i.e., ignore spaces only in text appearing right after <X>.
  if ($self->{'texinfo_ignore_leading_spaces'}) {
    $self->{'texinfo_ignore_leading_spaces'} = 0;
  }
  if ($context_tags{$tagname}) {
    if ($tagname eq 'L') {
      my $linktype = $attr_hash->{'type'};
      # set when the link text is redundant with the to argument.
      my $content_implicit = $attr_hash->{'content-implicit'};
      #print STDERR " L: $linktype CI: "
      #           .($content_implicit ? $content_implicit : 'NOCI')."\n";
      #my @attrs = keys %{$token->attr_hash};
      #print STDERR "  @attrs\n";
      #my $raw_L = $attr_hash->{'raw'}.'';
      #print STDERR " $attr_hash->{'raw'}: $raw_L\n";
      my ($url_arg, $texinfo_node, $texinfo_manual, $texinfo_section);
      if ($linktype eq 'man') {
        # NOTE: the .'' is here to force the $token->attr to ba a real
        # string and not an object.
        # NOTE 2: It is not clear that setting the url should be done
        # here, maybe this should be in the Texinfo HTML converter.
        # However, there is a 'man' category here and not in Texinfo,
        # so the information is more precise in pod.
        # NOTE 3: the section within the man (and not the numeric section in the
        # man page specification) which is in $attr_hash->{'section'} is ignored.
        # Maybe there would be a way to specify it, but it is not very important.
        my $replacement_arg = $attr_hash->{'to'}.'';
        # regexp from Pod::Simple::HTML resolve_man_page_link
        # since it is very small, it is likely that copyright cannot be
        # claimed for that part.
        $replacement_arg =~ /^([^(]+)(?:[(](\d+)[)])?$/;
        my $page = $1;
        my $section = $2;
        if (defined($page) and $page ne '') {
          $section = 1 if (!defined($section));
          # it is unlikely that there is a comma because of _url_escape
          # but to be sure there is still a call to _protect_comma.
          $url_arg = _protect_comma(_protect_text(
                                       $self->texinfo_man_url_prefix()
                                          ."$section/"._url_escape($page), 0, 1));
        } else {
          $url_arg = '';
        }
        $replacement_arg = _protect_text($replacement_arg);
        _output($fh, $self->{'texinfo_accumulated'}, "\@url{$url_arg,, $replacement_arg}");
      } elsif ($linktype eq 'url') {
        # NOTE: the .'' is here to force the $token->attr to be a real
        # string and not an object.
        $url_arg = _protect_comma(_protect_text($attr_hash->{'to'}.'', 0, 1));
      } elsif ($linktype eq 'pod') {
        # The section is available from $attr_hash->{'section'} as a
        # tree (not a token, a Pod::Simple::SimpleTree), or as a plain text
        # string with formatting removed when coerced to be a string.
        # Same for the name/manual obtained from $attr_hash->{'to'}.
        # The tokens obtained from the argument correspond, depending
        # on the cases, to 'name', or '"section" in name' (based on perlpodspec)
        # which is not practical for conversion to Texinfo as section and
        # name should be available separately, both converted to Texinfo.
        # It is possible to get the equivalent parsing in term of pod strings with
        # parselink(), which returns the same as the pullparser argument as
        # text, but also returns separately the section and name.
        # However, it is not possible to simply convert the section or
        # name string with the Texinfo pullparser parser, as a full pod text is
        # expected, starting whith a =head* while we would want to parse a
        # string only.
        #my ($l_text, $l_inferred, $l_name, $l_section, $l_type) = parselink($attr_hash->{'raw'});
        # To obtain the equivalent Texinfo, the Pod::Simple::SimpleTree
        # is converted with _convert_pod_simple_tree, which calls the
        # same functions as the pull parser implementation.
        my $manual = $attr_hash->{'to'};
        my $section = $attr_hash->{'section'};
        my ($section_text, $section_texi, $section_out);
        if (defined($section)) {
          # convert the section presented as tree to Texinfo
          _begin_context($self->{'texinfo_accumulated'}, 'L section');
          $self->_convert_pod_simple_tree($section);
          ($section_texi, $section_out)
            = _end_context($self->{'texinfo_accumulated'});
          # coerce to string
          $section_text = $section.'';
        }
        my ($manual_text, $manual_texi, $manual_out);
        if (defined($manual)) {
          # convert the manual presented as tree to Texinfo
          _begin_context($self->{'texinfo_accumulated'}, 'L manual');
          $self->_convert_pod_simple_tree($manual);
          ($manual_texi, $manual_out)
            = _end_context($self->{'texinfo_accumulated'});
          # coerce to string
          $manual_text = $manual.'';
        }
        if (0) {
        #if (1) {
          my $section_text_str = 'UNDEF';
          my $section_texi_str = 'UNDEF';
          if (defined($section_text)) {
            $section_text_str = $section_text;
            $section_texi_str = $section_texi;
          }
          $section_text_str = $section_text if (defined($section_text));
          my $manual_text_str = 'UNDEF';
          my $manual_texi_str = 'UNDEF';
          if (defined($manual_text)) {
            $manual_text_str = $manual_text;
            $manual_texi_str = $manual_texi;
          }
          print STDERR "L,M/S: $linktype $manual_text_str/$section_text_str\n";
          print STDERR "     texinfo:  $manual_texi_str/$section_texi_str\n";
        }
        if (defined($manual)) {
          if (! defined($section) or $section_text !~ m/\S/) {
            # use plain text string without formatting to match with what should
            # be given through texinfo_internal_pod_manuals().
            if ($self->{'texinfo_internal_pod_manuals_hash'}->{$manual_text}) {
              $section_texi = 'NAME';
              # use the manual name as texinfo section name, otherwise
              # it will be the section associated with the node, which is
              # the non informative 'NAME' section name
              $texinfo_section = _normalize_texinfo_name(
                 _protect_comma($manual_texi), 'section');
            }
          }
          # use plain text string without formatting to match with what should
          # be given through texinfo_internal_pod_manuals().
          if ($self->{'texinfo_internal_pod_manuals_hash'}->{$manual_text}) {
            $texinfo_node =
             _prepend_internal_section_manual($manual_texi, $section_texi,
                                 $self->texinfo_sectioning_base_level());
          } else {
            $texinfo_manual = _protect_text(
                        _pod_title_to_file_name($manual_text), 0, 1);
            if (defined($section)) {
              $texinfo_node = $section_texi;
            } else {
              $texinfo_node = '';
            }
          }
        } elsif (defined($section_text) and $section_text =~ m/\S/) {
          $texinfo_node =
           _prepend_internal_section_manual(
                                 $self->texinfo_short_title(), $section_texi,
                                 $self->texinfo_sectioning_base_level(), 1, 1);
          $texinfo_section = _normalize_texinfo_name(
                                _protect_comma($section_texi), 'section');
          #print STDERR "L: internal: $texinfo_node/$texinfo_section\n";
        }
        #print STDERR "L: not normalized node: $texinfo_node\n";
        $texinfo_node = _normalize_texinfo_name(
                _protect_colon(
                # FIXME remove end of lines?
                _protect_comma($texinfo_node)), 'anchor');
        #print STDERR "L: normalized node: $texinfo_node\n";

        # for pod, 'to' is the pod manual name.  Then 'section' is the
        # section.
      }
      push @{$self->{'texinfo_stack'}}, [$linktype, $content_implicit, $url_arg,
                           $texinfo_manual, $texinfo_node, $texinfo_section];
      #print STDERR join('|', @{$self->{'texinfo_stack'}->[-1]}) . "\n";
      #if (defined($to)) {
      #  print STDERR " | $to\n";
      #} else {
      #  print STDERR "\n";
      #}
      #print STDERR $token->dump."\n";
    }
    _begin_context($self->{'texinfo_accumulated'}, $tagname);
  } elsif ($tag_commands{$tagname}) {
    _output($fh, $self->{'texinfo_accumulated'}, "\@$tag_commands{$tagname}\{");
    if ($Texinfo::Common::brace_code_commands{$tag_commands{$tagname}}) {
      if (@{$self->{'texinfo_stack'}} and ref($self->{'texinfo_stack'}->[-1]) eq ''
          and defined($self->{'texinfo_raw_format_commands'}->{$self->{'texinfo_stack'}->[-1]})) {
        cluck "in $self->{'texinfo_stack'}->[-1]: $tagname $tag_commands{$tagname}";
      }
      push @{$self->{'texinfo_stack'}}, 'in_code';
    }
  } elsif ($environment_commands{$tagname}) {
    _output($fh, $self->{'texinfo_accumulated'}, "\@$environment_commands{$tagname}\n");
    if ($tagname eq 'Verbatim') {
      push @{$self->{'texinfo_stack'}}, 'verbatim';
    }
  } elsif ($tagname eq 'for') {
    my $target = $attr_hash->{'target'};
    push @{$self->{'texinfo_stack'}}, $target;
    if ($self->{'texinfo_raw_format_commands'}->{$target}) {
      _output($fh, $self->{'texinfo_accumulated'},
         "\@$self->{'texinfo_raw_format_commands'}->{$target}\n");
    } elsif ($self->{'texinfo_if_format_commands'}->{$target}) {
      _output($fh, $self->{'texinfo_accumulated'},
         "\@if$self->{'texinfo_if_format_commands'}->{$target}\n");
    }
  }
}

sub _texinfo_handle_text($$)
{
  my $self = shift;
  my $text = shift;

  my $fh = $self->{'output_fh'};

  # ignore spaces right after <X>
  if ($self->{'texinfo_ignore_leading_spaces'}) {
    $text =~ s/^\s*//;
    $self->{'texinfo_ignore_leading_spaces'} = 0;
  }
  my $result_text;
  if (@{$self->{'texinfo_stack'}} and ref($self->{'texinfo_stack'}->[-1]) eq ''
      and ((defined($self->{'texinfo_raw_format_commands'}->{$self->{'texinfo_stack'}->[-1]})
            and !$self->{'texinfo_raw_format_commands'}->{$self->{'texinfo_stack'}->[-1]})
           or ($self->{'texinfo_stack'}->[-1] eq 'verbatim'))) {
    $result_text = $text;
  } else {
    if (@{$self->{'texinfo_stack'}} and ref($self->{'texinfo_stack'}->[-1]) eq ''
        and ($self->{'texinfo_raw_format_commands'}->{$self->{'texinfo_stack'}->[-1]})) {
      $result_text = _protect_text($text, 0, 1);
      $result_text =~ s/^(\s*)#(\s*(line)? (\d+)(( "([^"]+)")(\s+\d+)*)?\s*)$/$1\@hashchar{}$2/mg;
    } else {
      $result_text = _protect_text($text, 0,
                (@{$self->{'texinfo_stack'}} and $self->{'texinfo_stack'}->[-1] eq 'in_code'));
    }
  }
  #print STDERR "T: !$text!->!$result_text!\n";
  _output($fh, $self->{'texinfo_accumulated'}, $result_text);
}

# tp remain compatible with PullParser, the $attr_hash should not be
# used, and everything passed through the stack.
sub _texinfo_handle_element_end($$$)
{
  my $self = shift;
  my $tagname = shift;
  my $attr_hash = shift;

  my $fh = $self->{'output_fh'};
  if ($context_tags{$tagname}) {
    # note that if the Pod command argument contains --- or -- they
    # will already have been protected as text with -@asis{}-, so
    # this will end up in the @anchor{} even if text protection
    # is considered in code for the @anchor{}.
    my ($result, $out) = _end_context($self->{'texinfo_accumulated'});
    #print STDERR "end: $tagname: $result, $out\n";
    my $texinfo_node = '';
    if ($line_commands{$tagname}) {

      my ($command, $command_argument);
      if ($pod_head_commands_level{$tagname}) {
        $command = $self->{'texinfo_head_commands'}->{$tagname};
      } elsif ($line_commands{$tagname}) {
        $command = $line_commands{$tagname};
      }

      if ($pod_head_commands_level{$tagname} or $tagname eq 'item-text') {
        chomp ($result);
        $result =~ s/\n/ /g;
        $result =~ s/^\s*//;
        $result =~ s/\s*$//;

        $command_argument = _normalize_texinfo_name($result, $command);
        if ($result =~ /\S/ and $command_argument !~ /\S/) {
          # use some raw text if the expansion lead to empty Texinfo code
          my $tree = parse_texi_line(undef, $result);
          my $converter = Texinfo::Convert::TextContent->converter();
          $command_argument = _protect_text($converter->convert_tree($tree));
        }

        if ($pod_head_commands_level{$tagname}
            and $pod_head_commands_level{$tagname} == 1
            and $standard_headers{lc($result)}) {
          # prepend the manual name for the top level texinfo section name for
          # internal manuals, otherwise the section name does not
          # allow to understand which module the following text refers to,
          # in particular for Info or PDF output based on the generated Texinfo.
          # Indeed, all the Pod files use the  same top level section names,
          # like NAME, METHODS, as described in perlpodstyle and based on
          # man pages conventions.
          $command_argument = _prepend_internal_section_manual(
                       $self->texinfo_short_title(), $command_argument,
                                $self->texinfo_sectioning_base_level(), 1);
        }

        my $anchor = '';
        my $node_name = _prepare_anchor($self, _node_name($self, $result));
        if ($node_name =~ /\S/) {
          if ($tagname eq 'item-text' or !$self->texinfo_section_nodes()) {
            $anchor = "\n\@anchor{$node_name}";
          } else {
            $texinfo_node = "\@node $node_name\n";
          }
        }
        $command_argument .= $anchor;
      } else {
        $command_argument = $result;
      }
      _output($fh, $self->{'texinfo_accumulated'},
              "$texinfo_node\@$command $command_argument\n$out\n");
    } elsif ($tagname eq 'Para') {
      _output($fh, $self->{'texinfo_accumulated'}, $out.
                               _protect_hashchar($result)."\n\n");
    } elsif ($tagname eq 'L') {
      my $format = pop @{$self->{'texinfo_stack'}};
      my ($linktype, $content_implicit, $url_arg,
          $texinfo_manual, $texinfo_node, $texinfo_section) = @$format;
      if ($linktype ne 'man') {
        my $explanation;
        if (defined($result) and $result =~ m/\S/ and !$content_implicit) {
          $explanation = ' '. _protect_comma($result);
        }
        if ($linktype eq 'url') {
          if (defined($explanation)) {
            _output($fh, $self->{'texinfo_accumulated'},
                    "\@url{$url_arg,$explanation}");
          } else {
            _output($fh, $self->{'texinfo_accumulated'}, "\@url{$url_arg}");
          }
        } elsif ($linktype eq 'pod') {
          if (defined($texinfo_manual)) {
            $explanation = '' if (!defined($explanation));
            _output($fh, $self->{'texinfo_accumulated'},
                     "\@ref{$texinfo_node,,$explanation, $texinfo_manual}");
          } elsif (defined($explanation)) {
            _output($fh, $self->{'texinfo_accumulated'},
                   "\@ref{$texinfo_node,,$explanation}");
          } else {
            if (defined($texinfo_section)
                and $texinfo_section ne $texinfo_node) {
              _output($fh, $self->{'texinfo_accumulated'},
                       "\@ref{$texinfo_node,, $texinfo_section}");
            } else {
              _output($fh, $self->{'texinfo_accumulated'},
                       "\@ref{$texinfo_node}");
            }
          }
        }
      }
    } elsif ($tagname eq 'X') {
      #my $next_token = $self->get_token();
      #if ($next_token) {
      #  if ($next_token->type() eq 'text') {
      #    my $next_text = $next_token->text;
      #    $next_text =~ s/^\s*//;
      #    $next_token->text($next_text);
      #    #_output($fh, $self->{'texinfo_accumulated'}, "\n");
      #  }
      #  $self->unget_token($next_token);
      #}
      $self->{'texinfo_ignore_leading_spaces'} = 1;
      chomp ($result);
      $result =~ s/\n/ /g;
      $result .= "\n";
      _output($fh, $self->{'texinfo_accumulated'}, "\@cindex $result", 1);
    }
  } elsif ($tag_commands{$tagname}) {
    _output($fh, $self->{'texinfo_accumulated'}, "}");
    if ($Texinfo::Common::brace_code_commands{$tag_commands{$tagname}}) {
      pop @{$self->{'texinfo_stack'}};
    }
  } elsif ($environment_commands{$tagname}) {
    if ($tagname eq 'Verbatim') {
      pop @{$self->{'texinfo_stack'}};
      _output($fh, $self->{'texinfo_accumulated'}, "\n");
    }
    my $tag = $environment_commands{$tagname};
    $tag =~ s/ .*//;
    _output($fh, $self->{'texinfo_accumulated'}, "\@end $tag\n\n");
  } elsif ($tagname eq 'for') {
    my $target = pop @{$self->{'texinfo_stack'}};
    if ($self->{'texinfo_raw_format_commands'}->{$target}) {
      _output($fh, $self->{'texinfo_accumulated'},
              "\n\@end $self->{'texinfo_raw_format_commands'}->{$target}\n");
    } elsif ($self->{'texinfo_if_format_commands'}->{$target}) {
      _output($fh, $self->{'texinfo_accumulated'},
              "\@end if$self->{'texinfo_if_format_commands'}->{$target}\n");
    }
  }
}

# does not appear as parsed token
# E entity/character
sub _convert_pod_tokens($)
{
  my $self = shift;

  my $fh = $self->{'output_fh'};

  $self->{'texinfo_accumulated'} = [];
  $self->{'texinfo_stack'} = [];
  while(my $token = $self->get_token()) {
    my $type = $token->type();
    #print STDERR "* type $type\n";
    #print STDERR $token->dump()."\n";
    if ($type eq 'start') {
      my $tagname = $token->tagname();
      $self->_texinfo_handle_element_start($tagname, $token->attr_hash());
    } elsif ($type eq 'text') {
      my $text = $token->text();
      $self->_texinfo_handle_text($text);
    } elsif ($type eq 'end') {
      my $tagname = $token->tagname();
      # attributes hash is not available in end token in PullParser
      my $attr_hash = {};
      $self->_texinfo_handle_element_end($tagname, $attr_hash);
    }
  }
}

sub _postamble($)
{
  my $self = shift;

  my $fh = $self->{'output_fh'};
  if ($self->texinfo_sectioning_base_level() == 0) {
    #print STDERR "$fh\n";
    print $fh "\@bye\n";
  }
}

# convert a tree, for instance a tree produced by
# Pod::Simple::SimpleTree, or Pod::Simple::LinkSection trees.
sub _convert_pod_simple_tree($$)
{
  my $self = shift;
  my $simple_tree = shift;

  my @simple_tree = @$simple_tree;
  my $tagname = shift @simple_tree;
  my $attr_hash = shift @simple_tree;
  $self->_texinfo_handle_element_start($tagname, $attr_hash);
  foreach my $content (@simple_tree) {
    if (ref($content) eq '') {
      $self->_texinfo_handle_text($content);
    } else {
      $self->_convert_pod_simple_tree($content);
    }
  }
  $self->_texinfo_handle_element_end($tagname, $attr_hash);
}

1;

__END__

=head1 NAME

Pod::Simple::Texinfo - format Pod as Texinfo

=head1 SYNOPSIS

  # From the command like
  perl -MPod::Simple::Texinfo -e Pod::Simple::Texinfo::go thingy.pod

  # From perl
  my $new = Pod::Simple::Texinfo->new;
  $new->texinfo_sectioning_style('unnumbered');
  my $from = shift @ARGV;
  my $to = $from;
  $to =~ s/\.(pod|pm)$/.texi/i;
  $new->parse_from_file($from, $to);

=head1 DESCRIPTION

This class is for making a Texinfo rendering of a Pod document.

This is a subclass of L<Pod::Simple::PullParser> and inherits all its
methods (and options).

It supports producing a standalone manual per Pod (the default) or
render the Pod as a chapter, see L</texinfo_sectioning_base_level>.

C<@documentencoding> is not output, which is consistent with outputting
Texinfo in utf8 in the caller.

=head1 METHODS

=over

=item texinfo_add_upper_sectioning_command

If set (the default case), a sectioning command is added at the beginning
of the output for the whole document, using the module name, at the level
above the level set by L<texinfo_sectioning_base_level>.  So there will be
a C<@part> if the level is equal to 1, a C<@chapter> if the level is equal
to 2 and so on and so forth.  If the base level is 0, a C<@top> command is
output instead.

=item texinfo_internal_pod_manuals

The argument should be a reference on an array containing the short
titles (usually the module names) of all the Pod documents that are
converted together and should be internal in the Texinfo document obtained
by including all those Pod documents.  References to those documents use
the internal reference commands formatting in Texinfo.  The formatting commands
should not be present in the short titles.

Corresponds to L<texinfo_sectioning_base_level> set to anything else than 0.

=item texinfo_main_command_sectioning_style

Sectioning style for the main command appearing at the beginning of the output
file if L<texinfo_sectioning_base_level> is anything else than 0.  Unset in the
default case.  If unset, use L<texinfo_sectioning_style>, except for style
C<heading>, for which the C<number> style is used in the default case.

=item texinfo_man_url_prefix

String used as a prefix for man page urls.  Default
is C<http://man.he.net/man>.

=item texinfo_section_nodes

If set, add C<@node> and not C<@anchor> for each sectioning command.

=item texinfo_sectioning_base_level

Sets the level of the head1 commands.  1 is for the @chapter/@unnumbered
level.  If set to 0, the head1 commands level is still 1, but the output
manual is considered to be a standalone manual.  If not 0, the Pod file is
rendered as a fragment of a Texinfo manual.

=item texinfo_sectioning_style

Default is C<numbered>, using the numbered sectioning Texinfo @-commands
(@chapter, @section...).  Giving C<unnumbered> leads to using unnumbered
sectioning command variants (C<@unnumbered>...), giving C<heading> leads to
using headings that are not associated with document structuring
(C<@heading>...) and any other value would lead to using appendix sectioning
command variants (C<@appendix>...).

=item texinfo_short_title

If set, used as short title.  Otherwise, set to the module name with
L<<< Pod::Simple::PullParser->get_short_title|Pod::Simple::PullParser/my $title_string = $parser->get_short_title >>>.
Can be accessed to get the module name associated with a C<Pod::Simple::Texinfo>
parser.

=back

=head1 SEE ALSO

L<Pod::Simple>. L<Pod::Simple::PullParser>. The Texinfo manual.

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2011, 2012 Free Software Foundation, Inc.

This library is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.

C<_url_escape> is C<general_url_escape> from L<Pod::Simple::HTML>.

=head1 AUTHOR

Patrice Dumas E<lt>pertusus@free.frE<gt>.  Parts from L<Pod::Simple::HTML>.

=cut
