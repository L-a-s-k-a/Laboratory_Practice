# Info.pm: output tree as Info.
#
# Copyright 2010-2022 Free Software Foundation, Inc.
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
#
# Names of methods from Texinfo::Convert::Plaintext overriden begin with format_.

package Texinfo::Convert::Info;

use 5.00405;
use strict;

# To check if there is no erroneous autovivification
#no autovivification qw(fetch delete exists store strict);

use Texinfo::Common;
use Texinfo::Convert::Plaintext;
use Texinfo::Convert::Text;

use Texinfo::Convert::Paragraph;

use vars qw($VERSION @ISA);
@ISA = qw(Texinfo::Convert::Plaintext);

$VERSION = '7.0.3';


my $STDIN_DOCU_NAME = 'stdin';

my %defaults = Texinfo::Convert::Plaintext::converter_defaults(undef, undef);
$defaults{'FORMAT_MENU'} = 'menu';
$defaults{'EXTENSION'} = 'info';
$defaults{'USE_SETFILENAME_EXTENSION'} = 1;
$defaults{'OUTFILE'} = undef;
# in the Emacs Info reader and in old readers, DEL character will appear,
# but the node names are problematic in those readers, so it is not
# such an issue to have them marked that way.
$defaults{'INFO_SPECIAL_CHARS_QUOTE'} = 1;
# set as default independently of INFO_SPECIAL_CHARS_QUOTE as long
# as the Emacs Info reader does not support node names quoting.
$defaults{'INFO_SPECIAL_CHARS_WARNING'} = 1;

sub converter_defaults($$)
{
  return %defaults;
}

sub output($)
{
  my $self = shift;
  my $root = shift;

  my $result;

  my ($output_file, $destination_directory, $output_filename,
     $document_name, $input_basefile) = $self->determine_files_and_directory();
  my ($encoded_destination_directory, $dir_encoding)
    = $self->encoded_output_file_name($destination_directory);
  my ($succeeded, $created_directory)
    = $self->create_destination_directory($encoded_destination_directory,
                                          $destination_directory);
  return undef unless $succeeded;

  # for format_node
  $self->{'output_filename'} = $output_filename;

  $input_basefile = $STDIN_DOCU_NAME if ($input_basefile eq '-');

  # no splitting when writing to the null device or to stdout
  if ($Texinfo::Common::null_device_file{$output_file}
       or $output_file eq '-') {
    $self->force_conf('SPLIT_SIZE', undef);
  }

  push @{$self->{'count_context'}}, {'lines' => 0, 'bytes' => 0,
                                     'locations' => []};
  my $header = $self->_info_header($input_basefile, $output_filename);
  # header + text between setfilename and first node
  my $complete_header = $header;

  pop @{$self->{'count_context'}};

  my $header_bytes = Texinfo::Convert::Plaintext::count_bytes($self, $header);
  my $complete_header_bytes = $header_bytes;
  my $tree_units = Texinfo::Structuring::split_by_node($root);

  my $fh;
  if (! $output_file eq '') {
    if ($self->get_conf('VERBOSE')) {
      print STDERR "Output file $output_file\n";
    }
    $fh = _open_info_file($self, $output_file);
    if (!$fh) {
      return undef;
    }
  }
  print STDERR "DOCUMENT\n" if ($self->get_conf('DEBUG'));
  my $out_file_nr = 0;
  my @indirect_files;
  if (!defined($tree_units) or not defined($tree_units->[0]->{'extra'})
      or not defined($tree_units->[0]->{'extra'}->{'unit_command'})) {
    $self->line_warn($self, __("document without nodes"),
             {'file_name' => $self->{'parser_info'}->{'input_file_name'}});
    my $output = $header.$self->convert_tree($root);
    $self->count_context_bug_message('no element ');

    my $footnotes = $self->process_footnotes();
    $self->count_context_bug_message('no element footnotes ');

    $output .= $footnotes;
    if ($fh) {
      print $fh $output;
    } else {
      $result = $output;
    }
  } else {
    unless ($self->{'structuring'} and $self->{'structuring'}->{'top_node'}
     and $self->{'structuring'}->{'top_node'}->{'extra'}->{'normalized'} eq 'Top') {
      $self->line_warn($self, __("document without Top node"),
             {'file_name' => $self->{'parser_info'}->{'input_file_name'}});
    }
    $out_file_nr = 1;
    my $first_node = 0;
    $self->{'count_context'}->[-1]->{'bytes'} += $header_bytes;
    my @nodes_root_elements = @$tree_units;
    while (@nodes_root_elements) {
      my $node_root_element = shift @nodes_root_elements;
      my $node_text = $self->convert_tree($node_root_element);
      if (!$first_node) {
        $first_node = 1;
        if (defined($self->{'text_before_first_node'})) {
          $complete_header .= $self->{'text_before_first_node'};
          $complete_header_bytes +=
            Texinfo::Convert::Plaintext::count_bytes($self,
                                   $self->{'text_before_first_node'});
        }
        # for the first node, header is prepended, not complete_header
        # as 'text_before_first_node' is already part of the node
        # text
        $node_text = $header . $node_text;
      }
      if ($fh) {
        print $fh $node_text;
      } else {
        $result .= $node_text;
      }
      $self->update_count_context();
      if (defined($self->get_conf('SPLIT_SIZE'))
          and $self->{'count_context'}->[-1]->{'bytes'} >
                  $out_file_nr * $self->get_conf('SPLIT_SIZE')
          and @nodes_root_elements and $fh) {
        my $close_error;
        if (!close ($fh)) {
          $close_error = $!;
        }
        if ($out_file_nr == 1) {
          $self->_register_closed_info_file($output_file);
          if (defined($close_error)) {
            $self->document_error($self,
                  sprintf(__("error on closing %s: %s"),
                                  $output_file, $close_error));
            return undef;
          }
          if ($self->get_conf('VERBOSE')) {
            print STDERR "Renaming first output file as ".
                  $output_file.'-'.$out_file_nr."\n";
          }
          unless (rename($output_file, $output_file.'-'.$out_file_nr)) {
            $self->document_error($self,
                  sprintf(__("rename %s failed: %s"),
                                         $output_file, $!));
            return undef;
          }
          # remove the main file from opened files since it was renamed
          # and add the file with a number.
          @{$self->{'opened_files'}} = grep {$_ ne $output_file}
               @{$self->{'opened_files'}};
          push @{$self->{'opened_files'}},
                   $output_file.'-'.$out_file_nr;
          push @indirect_files, [$output_filename.'-'.$out_file_nr,
                                 $complete_header_bytes];
          #print STDERR join(' --> ', @{$indirect_files[-1]}) ."\n";
        } else {
          $self->_register_closed_info_file($output_file.'-'.$out_file_nr);
          if (defined($close_error)) {
            $self->document_error($self,
                  sprintf(__("error on closing %s: %s"),
                                  $output_file.'-'.$out_file_nr,
                                  $close_error));
            return undef;
          }
        }
        $out_file_nr++;
        if ($self->get_conf('VERBOSE')) {
          print STDERR "New output file ".
                $output_file.'-'.$out_file_nr."\n";
        }
        $fh = _open_info_file($self, $output_file.'-'.$out_file_nr);
        if (!$fh) {
          return undef;
        }
        print $fh $complete_header;
        $self->update_count_context();
        $self->{'count_context'}->[-1]->{'bytes'} += $complete_header_bytes;
        push @indirect_files, [$output_filename.'-'.$out_file_nr,
                               $self->{'count_context'}->[-1]->{'bytes'}];
        #print STDERR join(' --> ', @{$indirect_files[-1]}) ."\n";
      }
    }
  }
  my $tag_text = '';
  if ($out_file_nr > 1) {
    $self->_register_closed_info_file($output_file.'-'.$out_file_nr);
    if (!close ($fh)) {
      $self->document_error($self,
               sprintf(__("error on closing %s: %s"),
                            $output_file.'-'.$out_file_nr, $!));
      return undef;
    }
    if ($self->get_conf('VERBOSE')) {
      print STDERR "Outputing the split manual file $output_file\n";
    }
    $fh = _open_info_file($self, $output_file);
    if (!$fh) {
      return undef;
    }
    $tag_text = $complete_header;
    $tag_text .= "\x{1F}\nIndirect:";
    foreach my $indirect (@indirect_files) {
      $tag_text .= "\n$indirect->[0]: $indirect->[1]";
    }
  }

  $tag_text .= "\n\x{1F}\nTag Table:\n";
  if ($out_file_nr > 1) {
    $tag_text .=  "(Indirect)\n";
  }
  # This may happen for anchors in @insertcopying
  my %seen_anchors;
  foreach my $label (@{$self->{'count_context'}->[-1]->{'locations'}}) {
    next unless ($label->{'root'} and $label->{'root'}->{'extra'}
                   and defined($label->{'root'}->{'extra'}->{'node_content'}));
    my $prefix;
    
    if ($label->{'root'}->{'cmdname'} eq 'node') {
      $prefix = 'Node';
    } else {
      $prefix = 'Ref';
    }
    my ($label_text, $byte_count) = $self->node_line($label->{'root'});

    if ($seen_anchors{$label_text}) {
      $self->converter_line_error($self, sprintf(__("\@%s output more than once: %s"),
          $label->{'root'}->{'cmdname'},
          Texinfo::Convert::Texinfo::convert_to_texinfo({'contents' =>
              $label->{'root'}->{'extra'}->{'node_content'}})),
        $label->{'root'}->{'source_info'});
      next;
    } else {
      $seen_anchors{$label_text} = 1;
    }

    $tag_text .=  "$prefix: $label_text\x{7F}$label->{'bytes'}\n";
  }
  $tag_text .=  "\x{1F}\nEnd Tag Table\n";
  my $coding = $self->get_conf('OUTPUT_ENCODING_NAME');
  if ($coding) {
    $tag_text .= "\n\x{1F}\nLocal Variables:\ncoding: $coding\nEnd:\n";
  }
  if ($fh) {
    print $fh $tag_text;
    # NOTE it should be possible to close STDOUT.  However this leads to
    # 'Filehandle STDOUT reopened as FH only for input' if there are files
    # reopened after closing STDOUT.  So closing STDOUT is handled by the
    # caller.
    unless ($output_file eq '-') {
      $self->_register_closed_info_file($output_file);
      if (!close ($fh)) {
        $self->document_error($self,
                  sprintf(__("error on closing %s: %s"),
                              $output_file, $!));
      }
    }
  } else {
    $result .= $tag_text;
  }
  return $result;
}

# Wrapper around Texinfo::Common::output_files_open_out.  Open the file
# with any CR-LF conversion disabled.  We need this for tag tables to
# be correct under MS-Windows.   Return filehandle or undef on failure.
sub _open_info_file($$)
{
  my $self = shift;
  my $filename = shift;

  my ($encoded_filename, $path_encoding)
      = $self->encoded_output_file_name($filename);
  my ($fh, $error_message) = Texinfo::Common::output_files_open_out(
                               $self->output_files_information(), $self,
                               $encoded_filename, 'use_binmode');

  if (!$fh) {
    $self->document_error($self, sprintf(
        __("could not open %s for writing: %s"),
        $filename, $error_message));
    return undef;
  }
  return $fh;
}

sub _register_closed_info_file($$)
{
  my $self = shift;
  my $filename = shift;

  my ($encoded_filename, $path_encoding)
      = $self->encoded_output_file_name($filename);

  Texinfo::Common::output_files_register_closed(
             $self->output_files_information(), $encoded_filename)
}

sub _info_header($$$)
{
  my $self = shift;
  my $input_basefile = shift;
  my $output_filename = shift;

  my $paragraph = Texinfo::Convert::Paragraph->new();
  my $result = add_text($paragraph, "This is ");
  # This ensures that spaces in file are kept.
  $result .= add_next($paragraph, $output_filename);
  my $program = $self->get_conf('PROGRAM');
  my $version = $self->get_conf('PACKAGE_VERSION');
  if (defined($program) and $program ne '') {
    $result .=
        add_text($paragraph, ", produced by $program version $version from ");
  } else {
    $result .= add_text($paragraph, ", produced from ");
  }
  $result .= add_next($paragraph, $input_basefile);
  $result .= add_text($paragraph, '.');
  $result .= Texinfo::Convert::Paragraph::end($paragraph);
  $result .= "\n";
  $self->{'empty_lines_count'} = 1;

  # format @copying using the last value of the preamble.
  my @informative_global_commands = $self->get_informative_global_commands();
  $self->set_global_document_commands('preamble', \@informative_global_commands);
  if ($self->{'global_commands'} and $self->{'global_commands'}->{'copying'}) {
    print STDERR "COPYING HEADER\n" if ($self->get_conf('DEBUG'));
    $self->{'in_copying_header'} = 1;
    my $copying = $self->convert_tree({'contents' =>
          $self->{'global_commands'}->{'copying'}->{'contents'}});
    $result .= $copying;
    $result .= $self->process_footnotes();
    delete $self->{'in_copying_header'};
  }
  $self->set_global_document_commands('before', \@informative_global_commands);

  if ($self->{'parser_info'}->{'dircategory_direntry'}) {
    $self->{'ignored_commands'}->{'direntry'} = 0;
    foreach my $command (@{$self->{'parser_info'}->{'dircategory_direntry'}}) {
      if ($command->{'cmdname'} eq 'dircategory') {
        if ($command->{'args'} and @{$command->{'args'}}
            and defined($command->{'args'}->[0]->{'contents'})) {
          my $dircategory = "INFO-DIR-SECTION ".$self->convert_line(
             {'contents' => $command->{'args'}->[0]->{'contents'}});
          $result .= $self->ensure_end_of_line($dircategory);
        }
        $self->{'empty_lines_count'} = 0;
      } elsif ($command->{'cmdname'} eq 'direntry') {
        $result .= "START-INFO-DIR-ENTRY\n";
        my $direntry = $self->convert_tree($command);
        $result .= $direntry;
        $result .= "END-INFO-DIR-ENTRY\n\n";
        $self->{'empty_lines_count'} = 1;
      }
    }
    $self->{'ignored_commands'}->{'direntry'} = 1;
  }
  return $result;
}

sub format_contents($$$)
{
  my $self = shift;
  my $section_root = shift;
  my $contents_or_shortcontents = shift;

  return ('', 0);
}

sub format_printindex($$)
{
  my $self = shift;
  my $printindex = shift;
  return $self->process_printindex($printindex, 1);
}

sub format_error_outside_of_any_node($$)
{
  my $self = shift;
  my $element = shift;
  if (!$self->{'current_node'}) {
    $self->converter_line_warn($self,
         sprintf(__("\@%s outside of any node"),
                 $element->{'cmdname'}), $element->{'source_info'});
  }
}

my @directions = ('Next', 'Prev', 'Up');
sub format_node($$)
{
  my $self = shift;
  my $node = shift;
  
  my $result = '';
  return '' if (!defined($node->{'extra'}->{'node_content'}));
  if (!$self->{'empty_lines_count'}) {
    $result .= "\n";
    $self->add_text_to_count("\n");
    # if in the first node, complete the 'text_before_first_node' too.
    if (!$self->{'first_node_done'}) {
      $self->{'text_before_first_node'} .= "\n";
    }
  }
  if (!$self->{'first_node_done'}) {
    $self->{'first_node_done'} = 1;
  }

  my $output_filename;
  if (defined($self->{'output_filename'})) {
    $output_filename = $self->{'output_filename'};
  } else {
    # May happen when only converting a fragment
    $output_filename = '';
  }

  $self->add_location($node);
  my $node_begin = "\x{1F}\nFile: $output_filename,  Node: ";
  $result .= $node_begin;
  $self->add_text_to_count($node_begin);
  my ($node_text, $byte_count) = $self->node_line($node);
  my $pre_quote = '';
  my $post_quote = '';
  if ($node_text =~ /,/) {
    if ($self->{'info_special_chars_warning'}) {
      $self->converter_line_warn($self, sprintf(__(
                 "\@node name should not contain `,': %s"), $node_text),
                               $node->{'source_info'});
    }
    if ($self->{'info_special_chars_quote'}) {
      $pre_quote = "\x{7f}";
      $post_quote = $pre_quote;
      $self->{'count_context'}->[-1]->{'bytes'} += 2;
    }
  }
  $self->{'count_context'}->[-1]->{'bytes'} += $byte_count;
  $result .= $pre_quote . $node_text . $post_quote;
  foreach my $direction (@directions) {
    if ($node->{'structure'}->{'node_'.lc($direction)}) {
      my $node_direction = $node->{'structure'}->{'node_'.lc($direction)};
      my $text = ",  $direction: ";
      $self->add_text_to_count($text);
      $result .= $text;
      if ($node_direction->{'extra'}->{'manual_content'}) {
        $result .= $self->convert_line({'type' => '_code',
                          'contents' => [{'text' => '('},
                             @{$node_direction->{'extra'}->{'manual_content'}},
                                          {'text' => ')'}]});
      }
      if ($node_direction->{'extra'}->{'node_content'}) {
        my $pre_quote = '';
        my $post_quote = '';
        my ($node_text, $byte_count) = $self->node_line($node_direction);
        $self->{'count_context'}->[-1]->{'bytes'} += $byte_count;
        # Up may not strictly need protection, as it is the last direction,
        # but we protect consistently
        if ($node_text =~ /,/) {
          if ($self->{'info_special_chars_warning'}
              # warn only for external nodes, internal nodes should already
              # trigger a warning when defined
              and $node_direction->{'extra'}->{'manual_content'}) {
            $self->converter_line_warn($self, sprintf(__(
                 "\@node %s name should not contain `,': %s"),
                                           $direction, $node_text),
                             $node->{'source_info'});
          }
          if ($self->{'info_special_chars_quote'}) {
            $pre_quote = "\x{7f}";
            $post_quote = $pre_quote;
            $self->{'count_context'}->[-1]->{'bytes'} += 2;
          }
        }
        $result .= $pre_quote . $node_text . $post_quote;
      }
    } elsif ($direction eq 'Up' and $node->{'extra'}->{'normalized'} eq 'Top') {
      # add an up direction for Top node
      my $text = ",  $direction: ".$self->get_conf('TOP_NODE_UP');
      $self->add_text_to_count($text);
      $result .= $text;
    }
  }
  $result .="\n\n";
  $self->add_text_to_count("\n\n");
  $self->{'count_context'}->[-1]->{'lines'} = 3;
  $self->{'empty_lines_count'} = 1;

  return $result;
}

my @image_files_extensions = ('.png', '.jpg');
sub format_image($$)
{
  my $self = shift;
  my $element = shift;
  my @extensions = @image_files_extensions;

  my $lines_count = 0;

  if (defined($element->{'args'}->[0])
      and $element->{'args'}->[0]->{'contents'}
      and @{$element->{'args'}->[0]->{'contents'}}) {
    my $basefile = Texinfo::Convert::Text::convert_to_text(
      {'contents' => $element->{'args'}->[0]->{'contents'}},
      {'code' => 1,
       Texinfo::Convert::Text::copy_options_for_convert_text($self)});
    if (defined($element->{'args'}->[4])
        and $element->{'args'}->[4]->{'contents'}
        and @{$element->{'args'}->[4]->{'contents'}}) {
      my $extension = Texinfo::Convert::Text::convert_to_text(
        {'contents' => $element->{'args'}->[4]->{'contents'}},
        {'code' => 1,
         Texinfo::Convert::Text::copy_options_for_convert_text($self)});
      unshift @extensions, ".$extension";
      unshift @extensions, "$extension";
    }
    my $image_file;
    foreach my $extension (@extensions) {
      my ($file_name, $file_name_encoding)
        = $self->encoded_input_file_name($basefile.$extension);
      if ($self->Texinfo::Common::locate_include_file($file_name)) {
        # use the basename and not the file found.  It is agreed that it is
        # better, since in any case the files are moved.
        # If the file path found was to be used it should be decoded to perl
        # codepoints too.
        $image_file = $basefile.$extension;
        last;
      }
    }
    my ($text, $width) = $self->txt_image_text($element, $basefile);
    my $alt;
    if (defined($element->{'args'}->[3])
        and $element->{'args'}->[3]->{'contents'}
        and @{$element->{'args'}->[3]->{'contents'}}) {
     $alt = Texinfo::Convert::Text::convert_to_text(
       {'contents' => $element->{'args'}->[3]->{'contents'}},
       {Texinfo::Convert::Text::copy_options_for_convert_text($self)});
    }

    my $result;

    if (defined($image_file) or (defined($text) and defined($alt))) {
      $image_file =~ s/\\/\\\\/g;
      $image_file =~ s/\"/\\\"/g;
      $result = "\x{00}\x{08}[image src=\"$image_file\"";

      if ($alt) {
        $alt =~ s/\\/\\\\/g;
        $alt =~ s/\"/\\\"/g;
        $result .= " alt=\"$alt\"";
      }
      if (defined($text)) {
        $text =~ s/\\/\\\\/g;
        $text =~ s/\"/\\\"/g;
        $result .= " text=\"$text\"";
      }
      $result .= "\x{00}\x{08}]";
      if ($self->{'formatters'}->[-1]->{'_top_formatter'}) {
        $result .= "\n";
      }
      my $image_lines_count = ($result =~ tr/\n/\n/) +1;
      $self->add_image($element, $image_lines_count, $width, 1);
    } else {
      $result = $self->image_formatted_text($element, $basefile, $text);
      $lines_count = ($result =~ tr/\n/\n/);
      $self->add_image($element, $lines_count+1, $width);
    }
    return ($result, $lines_count);
  }
  return ('', 0);
}

1;

__END__
# Automatically generated from maintain/template.pod

=head1 NAME

Texinfo::Convert::Info - Convert Texinfo tree to Info

=head1 SYNOPSIS

  my $converter
    = Texinfo::Convert::Info->converter({'parser' => $parser});

  $converter->output($tree);
  $converter->convert($tree);
  $converter->convert_tree($tree);

=head1 NOTES

The Texinfo Perl module main purpose is to be used in C<texi2any> to convert
Texinfo to other formats.  There is no promise of API stability.

=head1 DESCRIPTION

Texinfo::Convert::Info converts a Texinfo tree to Info.

=head1 METHODS

=over

=item $converter = Texinfo::Convert::Info->converter($options)

Initialize converter from Texinfo to Info.

The I<$options> hash reference holds options for the converter.  In
this option hash reference a L<parser object|Texinfo::Parser>
may be associated with the I<parser> key.  The other options
are Texinfo customization options and a few other options that can
be passed to the converter. Most of the customization options are described in
the Texinfo manual.  Those customization options, when appropriate, override
the document content.  The parser should not be available directly anymore
after getting the associated information.

See L<Texinfo::Convert::Converter> for more information.

=item $converter->output($tree)

Convert a Texinfo tree I<$tree> and output the result in files as
described in the Texinfo manual.

=item $result = $converter->convert($tree)

Convert a Texinfo tree I<$tree> and return the resulting output.

=item $result = $converter->convert_tree($tree)

Convert a Texinfo tree portion I<$tree> and return the resulting
output.  This function does not try to output a full document but only
portions.  For a full document use C<convert>.

=back

=head1 AUTHOR

Patrice Dumas, E<lt>pertusus@free.frE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright 2010- Free Software Foundation, Inc.  See the source file for
all copyright years.

This library is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at
your option) any later version.

=cut
