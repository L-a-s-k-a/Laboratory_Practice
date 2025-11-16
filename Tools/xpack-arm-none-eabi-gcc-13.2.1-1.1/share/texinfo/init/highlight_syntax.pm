# highlight_syntax.pm: interface to source-highlight for syntax highlighting
#
#    Copyright (C) 2021 Free Software Foundation, Inc.
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

require 5.0;
use strict;

# To check if there is no erroneous autovivification
#no autovivification qw(fetch delete exists store strict);

use File::Spec;

use Texinfo::Commands;
# also for __(
use Texinfo::Common;
use Texinfo::Convert::Text;
use Texinfo::Convert::NodeNameNormalization;

my %languages_name_mapping = (
  'C++' => 'C',
  'Perl' => 'perl',
);

my %languages_extensions = (
  'texinfo' => 'texi',
  'perl' => 'pl',
);

texinfo_add_valid_customization_option('HIGHLIGHT_SYNTAX_DEFAULT');

# reference on a hash
my $highlighted_languages_list;

# FIXME open shows an error message if the file is not found
# which is a duplicate with the texinfo_register_init_loading_error
# registered message, which is better
# Can't exec "source-highlight": No such file or directory at ./init/highlight_syntax.pm line 74
my $cmd = 'source-highlight --lang-list';
if (not(open(HIGHLIGHT_LANG_LIST, '-|', $cmd))) {
  texinfo_register_init_loading_error(
        sprintf(__('%s: %s'), $cmd, $!));
} else {
  $highlighted_languages_list = {};
  my $line;
  while (defined($line = <HIGHLIGHT_LANG_LIST>)) {
    chomp($line);
    if ($line =~ /^([A-Za-z0-9_\-]+) =/) {
       my $language = $1;
       $highlighted_languages_list->{$language} = 1;
    } else {
      texinfo_register_init_loading_warning(sprintf(__(
                        '%s: %s: cannot parse language line'), $cmd, $line))
    }
  }
  close(HIGHLIGHT_LANG_LIST);
}

if (defined($highlighted_languages_list)) {
  if (scalar(keys(%$highlighted_languages_list)) > 0) {
    texinfo_register_handler('structure', \&highlight_process);

    texinfo_register_command_formatting('example', \&highlight_preformatted_command);

    # normally this is done in preformatted type, but preformatted
    # types conversion output in example is discarded in
    # highlight_preformatted_command, so register a replacement.
    # Register inline pending content when opening an example block.
    texinfo_register_command_opening('example',
                                     \&highlight_open_inline_container_type);
  } else {
    # important if $cmd returns no output to have a message.  If there
    # is some output, there will already be some line parse error messages.
    texinfo_register_init_loading_warning(sprintf(__(
                              '%s: no highlighted language found'), $cmd));
  }
}

sub _get_language($$$)
{
  my $self = shift;
  my $cmdname = shift;
  my $command = shift;

  my $language;
  my $converted_language;

  if ($cmdname eq 'example') {
    if ($command->{'args'} and scalar(@{$command->{'args'}}) > 0) {
      $converted_language
        = Texinfo::Convert::NodeNameNormalization::convert_to_normalized(
                                                     $command->{'args'}->[0]);
      if ($converted_language eq '') {
        $converted_language = undef;
      }
    }
  }

  if (not defined($converted_language) and defined($self)) {
    my $default_highlight_language
      = $self->get_conf('HIGHLIGHT_SYNTAX_DEFAULT');
    if (defined($default_highlight_language)) {
      $converted_language = $default_highlight_language;
    }
  }

  if (defined($converted_language)
      and defined($languages_name_mapping{$converted_language})) {
    $language = $languages_name_mapping{$converted_language};
  } else {
    $language = $converted_language;
  }

  if ($highlighted_languages_list->{$language}) {
    return $language;
  } else {
    return undef;
  }
}

# the end of the string was randomly generated once for all.
my $range_separator
  = '_______________________________________ highlight texinfo _GT Haib0aik zei4YieH';

my %commands;

sub highlight_process($$)
{
  my $self = shift;
  my $document_root = shift;

  # initialization, important in case of multiple manuals processed
  %commands = ();              # associates a command name and element to the resulting
                               # highlighted text.
                               # Also holds per language counters.

  return 0 if (defined($self->get_conf('OUTFILE'))
        and $Texinfo::Common::null_device_file{$self->get_conf('OUTFILE')});

  my $document_name = $self->get_info('document_name');
  my $highlight_basename = "${document_name}_highlight";

  my $highlight_out_dir = $self->get_info('destination_directory');

  my @highlighted_commands = ('example');

  my $collected_commands
    = Texinfo::Common::collect_commands_in_tree($document_root,
                                             \@highlighted_commands);

  my %languages = ();
  foreach my $cmdname (@highlighted_commands) {
    if (scalar(@{$collected_commands->{$cmdname}}) > 0) {
      foreach my $element (@{$collected_commands->{$cmdname}}) {
        my $language = _get_language($self, $cmdname, $element);
        if (defined($language)) {
          $languages{$language} = {'counter' => 0, 'commands' => [],
                                   'line_ranges' => []}
            if (not exists($languages{$language}));
          $languages{$language}->{'counter'}++;
          my $counter = $languages{$language}->{'counter'};
          $languages{$language}->{'commands'}->[$counter-1]
                                                 = [$element, $cmdname];
          $commands{$cmdname} = {'input_languages_counters' => {},
                                 'results' => {},
                                 'retrieved_languages_counters'  => {},
                                 'output_languages_counters' => {}}
            if (not exists($commands{$cmdname}));
          if (not exists($commands{$cmdname}
                              ->{'input_languages_counters'}->{$language})) {
            $commands{$cmdname}->{'input_languages_counters'}->{$language} = 0;
            $commands{$cmdname}->{'retrieved_languages_counters'}->{$language} = 0;
            $commands{$cmdname}->{'output_languages_counters'}->{$language} = 0;
          }
        }
      }
    }
  }
  foreach my $language (keys(%languages)) {
    my $suffix;
    if (defined($languages_extensions{$language})) {
      $suffix = $languages_extensions{$language};
    } else {
      $suffix = $language
    }
    my $language_base = ${highlight_basename} . "_${language}";
    $languages{$language}->{'basefile'} = $language_base . "_input.$suffix";
    $languages{$language}->{'html_file'} = $language_base . '_output.html';
    my $input_language_path_name = File::Spec->catfile($highlight_out_dir,
                                $languages{$language}->{'basefile'});
    my $html_result_path_name = File::Spec->catfile($highlight_out_dir,
                                $languages{$language}->{'html_file'});

    # expand @example texts in an input file for highlight source
    # program
    my ($encoded_input_language_path_name, $input_language_path_encoding)
      = $self->encoded_output_file_name($input_language_path_name);
    unless (open (HIGHLIGHT_LANG_IN, ">$encoded_input_language_path_name")) {
      $self->document_warn($self,
             sprintf(__("highlight_syntax.pm: could not open %s: %s"),
                                      $input_language_path_name, $!));
      return 1;
    }
    my $output_encoding;
    if (defined($self->get_conf('OUTPUT_PERL_ENCODING'))) {
      $output_encoding = $self->get_conf('OUTPUT_PERL_ENCODING');
      binmode(HIGHLIGHT_LANG_IN, ":encoding($output_encoding)");
    }

    print HIGHLIGHT_LANG_IN "Automatically generated\n\n";
    my $highlight_lang_in_line_nr = 2;

    my $counter = 0;
    foreach my $element_command (@{$languages{$language}->{'commands'}}) {

      my $element = $element_command->[0];
      my $tree = {'contents' => [@{$element->{'contents'}}]};
      if ($tree->{'contents'}->[0]
          and $tree->{'contents'}->[0]->{'type'}
          and $tree->{'contents'}->[0]->{'type'} eq 'empty_line_after_command') {
        shift @{$tree->{'contents'}};
      }
      if ($tree->{'contents'}->[-1]->{'cmdname'}
          and $tree->{'contents'}->[-1]->{'cmdname'} eq 'end') {
        pop @{$tree->{'contents'}};
      }
      my $text = Texinfo::Convert::Text::convert_to_text($tree, {'code' => 1,
                  Texinfo::Convert::Text::copy_options_for_convert_text($self, 1)});
      # make sure that the text ends with a newline
      chomp ($text);
      $text .= "\n";
      # count the number of record separator $/
      my $buffer = $text;
      my $text_lines_nr = ( $buffer =~ s|$/||g );
      print HIGHLIGHT_LANG_IN "_______________________ $counter\n";
      print HIGHLIGHT_LANG_IN $text;
      print HIGHLIGHT_LANG_IN "_______________________ $counter\n";
      $languages{$language}->{'line_ranges'}->[$counter]
                    = [$highlight_lang_in_line_nr+1 +1,
                       $highlight_lang_in_line_nr + $text_lines_nr+1];
      $highlight_lang_in_line_nr += 2 + $text_lines_nr;
      $counter ++;
    }
    close(HIGHLIGHT_LANG_IN);

    # call source highlighting program
    my $version_option='';
    $version_option='--gen-version ' if ($self->get_conf('TEST'));
    my @option_line_ranges = ();
    foreach my $line_range (@{$languages{$language}->{'line_ranges'}}) {
      push @option_line_ranges, '"'.$line_range->[0].'-'.$line_range->[1].'"';
    }
    my $option_line_range_str = join(',', @option_line_ranges);
    my $cmd = "source-highlight ${version_option}"
       ."--src-lang=$language --out-format=html5 "
       ."-i '$input_language_path_name' -o '$html_result_path_name' "
   ."--line-range=$option_line_range_str --range-separator='$range_separator'";

    my $encoding = $self->get_conf('MESSAGE_ENCODING');
    my $encoded_cmd;
    if (defined($encoding)) {
      $encoded_cmd = encode($encoding, $cmd);
    } else {
      $encoded_cmd = $cmd;
    }
    if (system($encoded_cmd)) {
      $self->document_error($self,
          sprintf(__("highlight_syntax.pm: command did not succeed: %s"),
                                  $cmd));
      return 1;
    }

    my $language_fragments_nr = $languages{$language}->{'counter'};
    # extract highlighted fragments
    my ($encoded_html_result_path_name, $html_result_path_encoding)
      = $self->encoded_output_file_name($html_result_path_name);
    unless (open(HIGHLIGHT_LANG_OUT, $encoded_html_result_path_name)) {
      $self->document_warn($self,
         sprintf(__("highlight_syntax.pm: could not open %s: %s"),
                                  $html_result_path_name, $!));
      return 1;
    }
    binmode(HIGHLIGHT_LANG_OUT, ":encoding($output_encoding)")
      if (defined($output_encoding));
    my $got_count = 0;
    my $line;
    my $text;
    my $separators_count = 0;
    while ($line = <HIGHLIGHT_LANG_OUT>) {
      #print STDERR "$encoded_html_result_path_name: while $line";
      if ($line =~ /$range_separator/) {
        $separators_count++;
        if (defined($text)) {
          $got_count++;
          my $element_command
              = $languages{$language}->{'commands'}->[$got_count-1];
          my $element = $element_command->[0];
          my $cmdname = $element_command->[1];
          $commands{$cmdname}->{'results'}->{$element} = $text;
          $commands{$cmdname}->{'retrieved_languages_counters'}->{$language}++;
          $text = undef;
        }
        #print STDERR "$language $got_count $language_fragments_nr \n";
        if ($got_count < $language_fragments_nr) {
          $text = '';
        }
      } else {
        if (defined($text)) {
          $text .= $line;
        }
      }
    }
    if ($separators_count != $language_fragments_nr +1) {
      $self->document_warn($self, sprintf(__(
       "highlight_syntax.pm: %s: %d separators; expected %d, the number of fragments +1"),
                      $language, $separators_count, $language_fragments_nr+1));
    }
    if (defined($text) and $text ne '') {
      my $element_command = $languages{$language}->{'commands'}->[$got_count-1];
      my $element = $element_command->[0];
      my $cmdname = $element_command->[1];
      $self->document_warn($self, sprintf(__(
                 "highlight_syntax.pm: %s: end of \@%s item %d not found"),
                                  $language, $cmdname, $got_count));
    }
    # note that this check is not the most detailed that could be done, a check
    # by command could also be done.  Since for now there is only @example
    # it is useless, and even if there were other commands, the failure is
    # for a language, not a command, so it should not be needed either.
    if ($got_count != $languages{$language}->{'counter'}) {
      $self->document_warn($self, sprintf(__(
         "highlight_syntax.pm: %s: retrieved %d items in HTML; expected %d"),
                            $language, $got_count, $language_fragments_nr));
    }
    close (HIGHLIGHT_LANG_OUT);
  }
  return 0;
}

sub highlight_open_inline_container_type($$$)
{
  my $self = shift;
  my $cmdname = shift;
  my $command = shift;

  my $pending_formatted = $self->get_pending_formatted_inline_content();

  if (defined($pending_formatted)) {
    $self->associate_pending_formatted_inline_content($command,
                                                      $pending_formatted);
  }
  return '';
}

sub highlight_preformatted_command($$$$$)
{
  my $self = shift;
  my $cmdname = shift;;
  my $command = shift;
  my $args = shift;
  my $content = shift;

  # if no commands were registered nor converted, do not
  # warn if the language is known.  It means that there was
  # no highlighting or some error.
  if (exists ($commands{$cmdname})
      and exists ($commands{$cmdname}->{'results'})) {
    my $language = _get_language($self, $cmdname, $command);
    if (exists ($commands{$cmdname}->{'results'}->{$command})
        and defined($commands{$cmdname}->{'results'}->{$command})) {

      if (not defined($language)) {
        $self->document_warn($self, sprintf(__(
       "highlight_syntax.pm: output has HTML item for \@%s but no language %s"),
                                    $cmdname, $command));
      } else {
        $commands{$cmdname}->{'output_languages_counters'}->{$language}++;

        if ($self->in_string()) {
          return $content;
        }

        # need to do all the formatting done for content inside
        # of @example as it is discarded.  So need to do the preformatted
        # type formatting, from _convert_preformatted_type() and
        # _preformatted_class().
        # Since we are formatting @example itself, it is not in the preformatted
        # context anymore, so we readd.
        my @pre_classes = $self->preformatted_classes_stack();
        # NOTE $pre_class_format is setup below to correspond to
        # $pre_class_commands{$cmdname}, which cannot be used directly,
        # as it is private.
        my $pre_class_format = $cmdname;
        my $main_cmdname = $cmdname;
        if (defined($Texinfo::Common::small_block_associated_command{$cmdname})) {
          $pre_class_format
            = $Texinfo::Common::small_block_associated_command{$cmdname};
          $main_cmdname
            = $Texinfo::Common::small_block_associated_command{$cmdname};
        }
        push @pre_classes, $pre_class_format;
        my $pre_class;
        foreach my $class (@pre_classes) {
          # FIXME maybe add   or $pre_class eq 'menu'  to override
          # 'menu' with 'menu-comment'?
          $pre_class = $class unless ($pre_class
                 and $Texinfo::Commands::preformatted_code_commands{$pre_class}
                 and !($Texinfo::Commands::preformatted_code_commands{$class}
                                   or $class eq 'menu'));
        }
        $pre_class = $pre_class.'-preformatted';

        # Add classes as done in the default conversion function.
        # TODO is it correct?  What should be done with @example arguments?
        my @classes;
        if ($cmdname eq 'example') {
          if ($command->{'args'}) {
            for my $example_arg (@{$command->{'args'}}) {
              # convert or remove all @-commands, using simple ascii and unicode
              # characters
              my $converted_arg
               = Texinfo::Convert::NodeNameNormalization::convert_to_normalized(
                                                                   $example_arg);
              if ($converted_arg ne '') {
                push @classes, 'user-' . $converted_arg;
              }
            }
          }
        } elsif ($main_cmdname eq 'lisp') {
          push @classes, $main_cmdname;
          $main_cmdname = 'example';
        }
        unshift @classes, $main_cmdname;

        my $result_content = $commands{$cmdname}->{'results'}->{$command};
        # do it here, what was done in preformatted is discarded.
        # It should have been correctly registered
        # through highlight_open_inline_container_type.
        $result_content = $self->get_associated_formatted_inline_content($command)
                              . $result_content;
        $result_content =~ s/^\n/\n\n/; # a newline immediately after a <pre> is ignored.
        my $preformatted_result_content = $self->html_attribute_class('pre',
                                          [$pre_class]).">".$result_content."</pre>";
        return $self->html_attribute_class('div', \@classes).">\n"
               .$preformatted_result_content.'</div>'."\n";
      }
    # no error nor verbose message if there was no retrieved information
    # for that language
    } elsif (defined($language)
             and $commands{$cmdname}->{'retrieved_languages_counters'}->{$language}) {
      my $cmd_language_input_count
         = $commands{$cmdname}->{'input_languages_counters'}->{$language};
      my $cmd_language_retrieved_count
         = $commands{$cmdname}->{'retrieved_languages_counters'}->{$language};
      # Output an message only if the counters are equal, meaning language
      # was processed without failure.
      # If they are not equal there should have been a message already.
      if ($cmd_language_input_count == $cmd_language_retrieved_count) {
        $self->document_warn($self, sprintf(__(
                "highlight_syntax.pm: output has no HTML item for \@%s %s %s"),
                                    $cmdname, $language, $command));
      } elsif ($self->get_conf('VERBOSE') or $self->get_conf('DEBUG')) {
        warn "highlight_syntax.pm: output has no HTML item for \@$cmdname $language $command\n";
      }
    }
  }
  return &{$self->default_command_conversion($cmdname)}($self, $cmdname,
                                               $command, $args, $content);
}

1;
