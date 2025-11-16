# A style that tries to be analogous with a book, in HTML.
#
# Copyright 2004-2021 Free Software Foundation, Inc.
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
# Originally written by Patrice Dumas in 2004.
#
# This style is based on the scriptbasic style.

use strict;

# To check if there is no erroneous autovivification
#no autovivification qw(fetch delete exists store strict);

use Texinfo::Commands;
use Texinfo::Common;
use Texinfo::Convert::Texinfo;
use Texinfo::Structuring;

texinfo_set_from_init_file('contents', 1);
texinfo_set_from_init_file('CONTENTS_OUTPUT_LOCATION', 'inline');
texinfo_set_from_init_file('NO_TOP_NODE_OUTPUT', 1);
#texinfo_set_from_init_file('USE_TITLEPAGE_FOR_TITLE', 1);

my @book_buttons = ('Back', 'Forward', ' ', 'Contents', 'Index', 'About');

foreach my $buttons ('SECTION_BUTTONS', 'CHAPTER_BUTTONS', 'TOP_BUTTONS') {
  texinfo_set_from_init_file($buttons, \@book_buttons);
}

my @book_footer_buttons = ('Contents', 'Index', 'About');
foreach my $buttons ('MISC_BUTTONS', 'SECTION_FOOTER_BUTTONS',
                     'CHAPTER_FOOTER_BUTTONS') {
  texinfo_set_from_init_file($buttons, \@book_footer_buttons);
}

texinfo_set_from_init_file('NODE_FOOTER_BUTTONS', ['Back', 'Forward']);
texinfo_set_from_init_file('LINKS_BUTTONS',
    ['Top', 'Index', 'Contents', 'About', 'Up', 'NextFile', 'PrevFile']);

texinfo_set_from_init_file('WORDS_IN_PAGE', undef);
texinfo_set_from_init_file('FORMAT_MENU', 'nomenu');
texinfo_set_from_init_file('USE_NODES', 0);

texinfo_set_from_init_file('BIG_RULE', '<hr>');

my $toc_numbered_mark_class = 'toc-numbered-mark';

my ($book_previous_default_filename, $book_previous_file_name,
    $book_unumbered_nr);

sub book_init($)
{
  my $converter = shift;

  $book_previous_default_filename = undef;
  $book_previous_file_name = undef;
  $book_unumbered_nr = 0;
  return 0;
}

texinfo_register_handler('init', \&book_init);

sub book_print_up_toc($$)
{
  my $converter = shift;
  my $command = shift;

  my $result = '';
  my $current_command = $command;
  my @up_commands;
  while (defined($current_command->{'structure'}->{'section_up'})
           and ($current_command->{'structure'}->{'section_up'} ne $current_command)
           and defined($current_command->{'structure'}->{'section_up'}->{'cmdname'})) {
    unshift (@up_commands, $current_command->{'structure'}->{'section_up'});
    $current_command = $current_command->{'structure'}->{'section_up'};
  }
  # this happens for example for top tree unit
  return '' if !(@up_commands);
  my $up = shift @up_commands;
  #print STDERR "$up ".Texinfo::Convert::Texinfo::root_heading_command_to_texinfo($up)."\n";
  $result .= $converter->html_attribute_class('ul', [$toc_numbered_mark_class])."><li>"
  . "<a href=\"".$converter->command_href($up)."\">".$converter->command_text($up)
   . "</a> </li>\n";
  foreach my $up (@up_commands) {
    $result .= '<li>'
    .$converter->html_attribute_class('ul', [$toc_numbered_mark_class])."><li>"
    . "<a href=\"".$converter->command_href($up)."\">".$converter->command_text($up)
   . "</a> </li>\n";
  }
  foreach my $up (@up_commands) {
    $result .= "</ul></li>\n";
  }
  $result .= "</ul>\n";
  return $result;
}

sub book_format_navigation_header($$$$)
{
  my $self = shift;
  my $buttons = shift;
  my $cmdname = shift;
  my $element = shift;

  my $tree_unit = $element->{'structure'}->{'associated_unit'};
  if ($tree_unit and $tree_unit->{'extra'}->{'unit_command'}
      and not $tree_unit->{'extra'}->{'unit_command'}->{'cmdname'} eq 'node'
      and ($tree_unit->{'contents'}->[0] eq $element
          or (!$tree_unit->{'contents'}->[0]->{'cmdname'}
              and $tree_unit->{'contents'}->[1] eq $element))
      and defined($tree_unit->{'structure'}->{'unit_filename'})
      and $self->count_elements_in_filename('current',
                         $tree_unit->{'structure'}->{'unit_filename'}) == 1) {
    return book_print_up_toc($self, $tree_unit->{'extra'}->{'unit_command'}) .
       &{$self->default_formatting_function('format_navigation_header')}($self,
                                 $buttons, $cmdname, $element);

  } else {
    return &{$self->default_formatting_function('format_navigation_header')}(
                                          $self, $buttons, $cmdname, $element);
  }
}

texinfo_register_formatting_function('format_navigation_header',
                                     \&book_format_navigation_header);

sub book_print_sub_toc($$$);

sub book_print_sub_toc($$$)
{
  my $converter = shift;
  my $parent_command = shift;
  my $command = shift;

  my $result = '';
  my $content_href = $converter->command_href($command);
  my $heading = $converter->command_text($command);
  if ($content_href) {
    $result .= "<li> "."<a href=\"$content_href\">$heading</a>" . " </li>\n";
  }
  if ($command->{'structure'}->{'section_childs'}
      and @{$command->{'structure'}->{'section_childs'}}) {
    $result .= '<li>'.$converter->html_attribute_class('ul', [$toc_numbered_mark_class])
     .">\n". book_print_sub_toc($converter, $parent_command,
                                $command->{'structure'}->{'section_childs'}->[0])
     ."</ul></li>\n";
  }
  if (exists($command->{'structure'}->{'section_next'})) {
    $result .= book_print_sub_toc($converter, $parent_command,
                                  $command->{'structure'}->{'section_next'});
  }
  return $result;
}

# this function is very similar with the default function, but there is
# an additional sub toc before the content.  It should be synced with
# the default function.
sub book_convert_heading_command($$$$$)
{
  my $self = shift;
  my $cmdname = shift;
  my $element = shift;
  my $args = shift;
  my $content = shift;

  my $result = '';

  # No situation where this could happen
  if ($self->in_string) {
    $result .= $self->command_text($element, 'string') ."\n"
      if ($cmdname ne 'node');
    $result .= $content if (defined($content));
    return $result;
  }

  my $element_id = $self->command_id($element);

  print STDERR "CONVERT elt heading $element "
        .Texinfo::Convert::Texinfo::root_heading_command_to_texinfo($element)."\n"
          if ($self->get_conf('DEBUG'));
  my $tree_unit;
  if ($Texinfo::Commands::root_commands{$element->{'cmdname'}}
      and $element->{'structure'}->{'associated_unit'}) {
    $tree_unit = $element->{'structure'}->{'associated_unit'};
  }
  my $element_header = '';
  if ($tree_unit) {
    $element_header = &{$self->formatting_function('format_element_header')}(
                                        $self, $cmdname, $element, $tree_unit);
  }

  my $tables_of_contents = '';
  my $structuring = $self->get_info('structuring');
  if ($self->get_conf('CONTENTS_OUTPUT_LOCATION') eq 'after_top'
      and $cmdname eq 'top'
      and $structuring and $structuring->{'sectioning_root'}
      and scalar(@{$structuring->{'sections_list'}}) > 1) {
    foreach my $content_command_name ('contents', 'shortcontents') {
      if ($self->get_conf($content_command_name)) {
        my $contents_text
          = $self->_contents_inline_element($content_command_name, undef);
        if ($contents_text ne '') {
          $tables_of_contents .= $contents_text;
        }
      }
    }
  }

  my $sub_toc = '';
  if ($tables_of_contents eq ''
      and $element->{'structure'}->{'section_childs'}
      and @{$element->{'structure'}->{'section_childs'}}
      # FIXME why not @top?
      and $cmdname ne 'top'
      and $Texinfo::Commands::sectioning_heading_commands{$cmdname}) {
    $sub_toc .= $self->html_attribute_class('ul', [$toc_numbered_mark_class]).">\n";
    $sub_toc .= book_print_sub_toc($self, $element,
                                  $element->{'structure'}->{'section_childs'}->[0]);
    $sub_toc .= "</ul>\n";
  }

  if ($self->get_conf('NO_TOP_NODE_OUTPUT')
      and $Texinfo::Commands::root_commands{$cmdname}) {
    my $in_skipped_node_top
      = $self->shared_conversion_state('in_skipped_node_top', 0);
    if ($cmdname eq 'node') {
      if ($$in_skipped_node_top == 0
          and $element->{'extra'}
          and $element->{'extra'}->{'normalized'} eq 'Top') {
        $$in_skipped_node_top = 1;
      } elsif ($$in_skipped_node_top == 1) {
        $$in_skipped_node_top = -1;
      }
    }
    if ($$in_skipped_node_top == 1) {
      my $id_class = $cmdname;
      $result .= &{$self->formatting_function('format_separate_anchor')}($self,
                                                        $element_id, $id_class);
      $result .= $element_header;
      $result .= $tables_of_contents;
      $result .= $sub_toc;
      return $result;
    }
  }

  my @heading_classes;
  my $level_corrected_cmdname = $cmdname;
  if (defined $element->{'structure'}->{'section_level'}) {
    # if the level was changed, use a consistent command name
    $level_corrected_cmdname
      = Texinfo::Structuring::section_level_adjusted_command_name($element);
    if ($level_corrected_cmdname ne $cmdname) {
      push @heading_classes,
            "${cmdname}-level-set-${level_corrected_cmdname}";
    }
  }

  # find the section starting here, can be through the associated node
  # preceding the section, or the section itself
  my $opening_section;
  my $level_corrected_opening_section_cmdname;
  if ($cmdname eq 'node' and $element->{'extra'}->{'associated_section'}) {
    $opening_section = $element->{'extra'}->{'associated_section'};
    $level_corrected_opening_section_cmdname
     = Texinfo::Structuring::section_level_adjusted_command_name($opening_section);
  } elsif ($cmdname ne 'node'
           # if there is an associated node, it is not a section opening
           # the section was opened before when the node was encountered
           and not $element->{'extra'}->{'associated_node'}
           # to avoid *heading* @-commands
           and $Texinfo::Commands::root_commands{$cmdname}) {
    $opening_section = $element;
    $level_corrected_opening_section_cmdname = $level_corrected_cmdname;
  }

  # $heading not defined may happen if the command is a @node, for example
  # if there is an error in the node.
  my $heading = $self->command_text($element);
  my $heading_level;
  # node is used as heading if there is nothing else.
  if ($cmdname eq 'node') {
    # FIXME what to do if the $tree_unit extra does not contain any
    # unit_command, but tree_unit is defined (it can contain only 'first_in_page')
    if ((!$tree_unit # or !$tree_unit->{'extra'}
         # or !$tree_unit->{'extra'}->{'unit_command'}
         or ($tree_unit->{'extra'}->{'unit_command'}
             and $tree_unit->{'extra'}->{'unit_command'} eq $element
             and not $element->{'extra'}->{'associated_section'}))
        and defined($element->{'extra'}->{'normalized'})) {
      if ($element->{'extra'}->{'normalized'} eq 'Top') {
        $heading_level = 0;
      } else {
        $heading_level = 3;
      }
    }
  } elsif (defined $element->{'structure'}->{'section_level'}) {
    $heading_level = $element->{'structure'}->{'section_level'};
  } else {
    # for *heading* @-commands which do not have a level
    # in the document as they are not associated with the
    # sectioning tree, but still have a $heading_level
    $heading_level = Texinfo::Common::section_level($element);
  }

  my $do_heading = (defined($heading) and $heading ne ''
                    and defined($heading_level));

  # if set, the id is associated to the heading text
  my $heading_id;
  if ($opening_section) {
    my $level = $opening_section->{'structure'}->{'section_level'};
    $result .= join('', $self->close_registered_sections_level($level));
    $self->register_opened_section_level($level, "</div>\n");

    # use a specific class name to mark that this is the start of
    # the section extent. It is not necessary where the section is.
    $result .= $self->html_attribute_class('div',
                 ["${level_corrected_opening_section_cmdname}-level-extent"]);
    $result .= " id=\"$element_id\""
        if (defined($element_id) and $element_id ne '');
    $result .= ">\n";
  } elsif (defined($element_id) and $element_id ne '') {
    if ($element_header ne '') {
      # case of a @node without sectioning command and with a header.
      # put the node element anchor before the header.
      # Set the class name to the command name if there is no heading,
      # else the class will be with the heading element.
      my $id_class = $cmdname;
      if ($do_heading) {
        $id_class = "${cmdname}-id";
      }
      $result .= &{$self->formatting_function('format_separate_anchor')}($self,
                                                        $element_id, $id_class);
    } else {
      $heading_id = $element_id;
    }
  }

  $result .= $element_header;

  if ($do_heading) {
    if ($self->get_conf('TOC_LINKS')
        and $Texinfo::Commands::root_commands{$cmdname}
        and $Texinfo::Commands::sectioning_heading_commands{$cmdname}) {
      my $content_href = $self->command_contents_href($element, 'contents');
      if ($content_href ne '') {
        $heading = "<a href=\"$content_href\">$heading</a>";
      }
    }


    my $heading_class = $level_corrected_cmdname;
    unshift @heading_classes, $heading_class;
    if ($self->in_preformatted()) {
      my $id_str = '';
      if (defined($heading_id)) {
        $id_str = " id=\"$heading_id\"";
      }
      $result .= $self->html_attribute_class('strong', \@heading_classes)
                                   ."${id_str}>".$heading.'</strong>'."\n";
    } else {
      $result .= &{$self->formatting_function('format_heading_text')}($self,
                     $level_corrected_cmdname, \@heading_classes, $heading,
                     $heading_level +$self->get_conf('CHAPTER_HEADER_LEVEL') -1,
                     $heading_id, $element);
    }
  } elsif (defined($heading_id)) {
    # case of a lone node and no header, and case of an empty @top
    $result .= &{$self->formatting_function('format_separate_anchor')}($self,
                                                       $heading_id, $cmdname);
  }

  $result .= $tables_of_contents;
  $result .= $sub_toc;
  $result .= $content if (defined($content));

  return $result;
}

foreach my $command (keys(%Texinfo::Commands::sectioning_heading_commands),
                                                                  'node') {
  texinfo_register_command_formatting($command,
                                \&book_convert_heading_command);
}

sub book_element_file_name($$$$)
{
  my $converter = shift;
  my $element = shift;
  my $filename = shift;
  my $filepath = shift;

  return (undef, undef) if (!$converter->get_conf('SPLIT'));

  # should only happen if ! SPLIT, so should be redundant with the
  # condition above
  return ($filename, $filepath) if (defined($filepath));

  if (defined($book_previous_default_filename)
      and ($filename eq $book_previous_default_filename)) {
    return ($book_previous_file_name, undef);
  }

  my $prefix = $converter->{'document_name'};
  my $new_file_name;
  my $command;
  if ($element->{'extra'}->{'unit_command'}) {
    if ($element->{'extra'}->{'unit_command'}->{'cmdname'} ne 'node') {
      $command = $element->{'extra'}->{'unit_command'};
    } elsif ($element->{'extra'}->{'unit_command'}->{'extra'}
             and $element->{'extra'}->{'unit_command'}->{'extra'}->{'associated_section'}) {
      $command = $element->{'extra'}->{'unit_command'}->{'extra'}->{'associated_section'};
    }
  }
  return undef unless ($command);
  if ($converter->element_is_tree_unit_top($element)) {
    $new_file_name = "${prefix}_top.html";
  } elsif (defined($command->{'structure'}->{'section_number'})
           and ($command->{'structure'}->{'section_number'} ne '')) {
    my $number = $command->{'structure'}->{'section_number'};
    $number .= '.' unless ($number =~ /\.$/);
    $new_file_name = "${prefix}_$number" . 'html';
  } else {
    $book_unumbered_nr++;
    $new_file_name = "${prefix}_U." . $book_unumbered_nr . '.html';
  }
  $book_previous_default_filename = $filename;
  $book_previous_file_name = $new_file_name;
  return ($new_file_name, undef);
}

texinfo_register_file_id_setting_function('tree_unit_file_name',
                                          \&book_element_file_name);

1;
