# Transformations.pm: some transformations of the document tree
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
# Parts (also from Patrice Dumas) come from texi2html.pl.

package Texinfo::Transformations;

use 5.00405;

use strict;

# To check if there is no erroneous autovivification
#no autovivification qw(fetch delete exists store strict);


use Carp qw(cluck);

use Texinfo::Commands;
use Texinfo::Common;
use Texinfo::Translations;
use Texinfo::Structuring;

require Exporter;
use vars qw($VERSION @ISA @EXPORT_OK %EXPORT_TAGS);
@ISA = qw(Exporter);

%EXPORT_TAGS = ( 'all' => [ qw(
protect_hashchar_at_line_beginning
reference_to_arg_in_tree
) ] );

@EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

$VERSION = '7.0.3';

# Add raise/lowersections to be back at the normal level
sub _correct_level($$;$)
{
  my $section = shift;
  my $parent = shift;
  my $modifier = shift;
  $modifier = 1 if (!defined($modifier));

  my @result;
  if ($section->{'extra'} and $section->{'extra'}->{'sections_level'}) {
    my $level_to_remove = $modifier * $section->{'extra'}->{'sections_level'};
    my $command;
    if ($level_to_remove < 0) {
      $command = 'raisesection';
    } else {
      $command = 'lowersection';
    }
    my $remaining_level = abs($level_to_remove);
    while ($remaining_level) {
      push @result, {'cmdname' => $command,
                     'parent' => $parent};
      push @result, {'type' => 'empty_line', 'text' => "\n",
                     'parent' => $parent};
      $remaining_level--;
    }
  }
  return @result;
}

sub fill_gaps_in_sectioning($)
{
  my $root = shift;
  my @sections_list;
  foreach my $content (@{$root->{'contents'}}) {
    if ($content->{'cmdname'} and $content->{'cmdname'} ne 'node'
        and $Texinfo::Commands::root_commands{$content->{'cmdname'}}) {
      push @sections_list, $content;
    }
  }

  return (undef, undef) if (!scalar(@sections_list));

  my @added_sections;
  my @contents;
  my $previous_section;
  foreach my $content(@{$root->{'contents'}}) {
    push @contents, $content;
    if (!@sections_list or $sections_list[0] ne $content) {
      next;
    }
    my $current_section = shift @sections_list;
    my $current_section_level
       = Texinfo::Common::section_level($current_section);
    my $next_section = $sections_list[0];
    
    if (defined($next_section)) {
      my $next_section_level
                        = Texinfo::Common::section_level($next_section);

      if ($next_section_level - $current_section_level > 1) {
        my @correct_level_offset_commands = _correct_level($next_section,
                                                          $contents[-1]);
        if (@correct_level_offset_commands) {
          push @{$contents[-1]->{'contents'}}, @correct_level_offset_commands;
        }
        while ($next_section_level - $current_section_level > 1) {
          $current_section_level++;
          my $new_section = {'cmdname' =>
            $Texinfo::Common::level_to_structuring_command{'unnumbered'}->[$current_section_level],
            'parent' => $root,
          };
          $new_section->{'contents'} = [{'type' => 'empty_line',
                                         'text' => "\n",
                                         'parent' => $new_section}];
          $new_section->{'args'} = [{'type' => 'line_arg',
                                     'parent' => $new_section}];
          $new_section->{'args'}->[0]->{'contents'} = [
             {'cmdname' => 'asis',
              'parent' => $new_section->{'args'}->[0]
             },
             {'type' => 'spaces_at_end',
              'text' => "\n",
              'parent' => $new_section->{'args'}->[0]
             }];
          $new_section->{'args'}->[0]->{'extra'}
            = {'spaces_before_argument' => ' '};
          $new_section->{'args'}->[0]->{'contents'}->[0]->{'args'}
             = [{'type' => 'brace_command_arg',
                 'contents' => [],
                 'parent' => $new_section->{'args'}->[0]->{'contents'}->[1]}];
          push @contents, $new_section;
          push @added_sections, $new_section;
        }
        my @set_level_offset_commands = _correct_level($next_section,
                                                       $contents[-1], -1);
        if (@set_level_offset_commands) {
          push @{$contents[-1]->{'contents'}}, @set_level_offset_commands;
        }
      }
    }
  }
  return (\@contents, \@added_sections);
}

# This converts a reference @-command to simple text using one of the
# arguments.  This is used to remove reference @-command from
# constructed node names trees, as node names cannot contain
# reference @-command while there could be some in the tree used in
# input for the node name tree.
sub _reference_to_arg($$$)
{
  my $type = shift;
  my $current = shift;

  if ($current->{'cmdname'} and
      $Texinfo::Commands::ref_commands{$current->{'cmdname'}}
      and $current->{'args'}) {
    my @args_try_order;
    if ($current->{'cmdname'} eq 'inforef') {
      @args_try_order = (0, 1, 2);
    } else {
      @args_try_order = (0, 1, 2, 4, 3);
    }
    foreach my $index (@args_try_order) {
      if (defined($current->{'args'}->[$index])) {
        # This a double checking that there is some content.
        # Not sure that it is useful.
        my $text = Texinfo::Convert::Text::convert_to_text(
                                           $current->{'args'}->[$index]);
        if (defined($text) and $text =~ /\S/) {
          my $result
            = {'contents' => $current->{'args'}->[$index]->{'contents'},
                        'parent' => $current->{'parent'}};
          return ($result);
        }
      }
    }
    return {'text' => '', 'parent' => $current->{'parent'}};
  } else {
    return ($current);
  }
}

sub reference_to_arg_in_tree($)
{
  my $tree = shift;
  return Texinfo::Common::modify_tree($tree, \&_reference_to_arg);
}

# prepare a new node
# modifies $nodes_list, $targets_list, $labels
sub _new_node($$$$)
{
  my $node_tree = shift;
  my $nodes_list = shift;
  my $targets_list = shift;
  my $labels = shift;

  # We protect for all the contexts, as the node name should be
  # the same in the different contexts, even if some protections
  # are not needed for the parsing.  Also, this way the node tree
  # can be directly reused in the menus for example, without
  # additional protection, some parts could be double protected
  # otherwise, those that are protected with @asis.
  #
  # needed in nodes lines, @*ref and in menus with a label
  $node_tree = Texinfo::Common::protect_comma_in_tree($node_tree);
  # always
  $node_tree->{'contents'}
   = Texinfo::Common::protect_first_parenthesis($node_tree->{'contents'})
     if ($node_tree->{'contents'});
  # in menu entry without label
  $node_tree = Texinfo::Common::protect_colon_in_tree($node_tree);
  # in menu entry with label
  $node_tree = Texinfo::Common::protect_node_after_label_in_tree($node_tree);
  $node_tree = reference_to_arg_in_tree($node_tree);

  my $empty_node = 0;
  if (!$node_tree->{'contents'}
      or !scalar(@{$node_tree->{'contents'}})) {
    $node_tree->{'contents'} = [{'text' => ''}];
    $empty_node = 1;
  }

  my $comment_at_end;
  if ($node_tree->{'contents'}->[-1]->{'cmdname'}
      and ($node_tree->{'contents'}->[-1]->{'cmdname'} eq 'c'
           or $node_tree->{'contents'}->[-1]->{'cmdname'} eq 'comment')) {
    $comment_at_end = pop @{$node_tree->{'contents'}};
  }
  my $spaces_after_argument = '';
  if (scalar(@{$node_tree->{'contents'}}) > 0
             and $node_tree->{'contents'}->[-1]->{'text'}
             and $node_tree->{'contents'}->[-1]->{'text'} =~ s/(\s+)$//) {
    $spaces_after_argument = $1;
  }
  $spaces_after_argument .= "\n" unless ($spaces_after_argument =~ /\n/
                                         or $comment_at_end);

  my $appended_number = 0 +$empty_node;
  my ($node, $parsed_node);

  while (!defined($node)
         or ($labels
            and $labels->{$parsed_node->{'normalized'}})) {
    $node = {'cmdname' => 'node',
             'args' => [
               {'type' => 'line_arg',
                'extra' =>
                    {'spaces_after_argument' => $spaces_after_argument}}
             ],
             'extra' => {'spaces_before_argument' => ' '}};
    my $node_line_arg = $node->{'args'}->[0];
    $node_line_arg->{'parent'} = $node;
    $node_line_arg->{'extra'}->{'comment_at_end'} = $comment_at_end
      if (defined($comment_at_end));
    @{$node_line_arg->{'contents'}} = (@{$node_tree->{'contents'}});
    if ($appended_number) {
      push @{$node_line_arg->{'contents'}}, {'text' => " $appended_number"};
    }
    foreach my $content (@{$node_line_arg->{'contents'}}) {
      $content->{'parent'} = $node_line_arg;
    }
    my $modified_node_content;
    ($parsed_node, $modified_node_content)
       = Texinfo::Common::parse_node_manual($node_line_arg);
    if ($parsed_node and $parsed_node->{'node_content'}) {
      $parsed_node->{'normalized'} =
       Texinfo::Convert::NodeNameNormalization::normalize_node(
        { 'contents' => $parsed_node->{'node_content'} });
    }
    $node_line_arg->{'contents'} = $modified_node_content;
    if (!defined($parsed_node) or !$parsed_node->{'node_content'}
        or $parsed_node->{'normalized'} !~ /[^-]/) {
      if ($appended_number) {
        return undef;
      } else {
        $node = undef;
      }
    }
    $appended_number++;
  }

  push @{$node->{'extra'}->{'nodes_manuals'}}, $parsed_node;
  if ($parsed_node->{'normalized'} ne '') {
    $labels->{$parsed_node->{'normalized'}} = $node;
    $node->{'extra'}->{'normalized'} = $parsed_node->{'normalized'};
  }
  Texinfo::Common::register_label($targets_list, $node, $parsed_node);
  push @{$nodes_list}, $node;
  return $node;
}

# reassociate a tree element to the new node, from previous node
sub _reassociate_to_node($$$$)
{
  my $type = shift;
  my $current = shift;
  my $argument = shift;
  my ($new_node, $previous_node) = @{$argument};

  if ($current->{'cmdname'} and $current->{'cmdname'} eq 'menu') {
    if ($previous_node) {
      if (not defined($previous_node->{'extra'}->{'menus'})
          or not scalar(@{$previous_node->{'extra'}->{'menus'}})
          or not (grep {$current eq $_} @{$previous_node->{'extra'}->{'menus'}})) {
        print STDERR "Bug: menu $current not in previous node $previous_node\n";
      } else {
        @{$previous_node->{'extra'}->{'menus'}}
          = grep {$_ ne $current} @{$previous_node->{'extra'}->{'menus'}};
        delete $previous_node->{'extra'}->{'menus'} if !(@{$previous_node->{'extra'}->{'menus'}});
      }
    }
    push @{$new_node->{'extra'}->{'menus'}}, $current;
  } elsif ($current->{'extra'} and $current->{'extra'}->{'index_entry'}) {
    if ($previous_node
        and (!$current->{'extra'}->{'index_entry'}->{'entry_node'}
             or $current->{'extra'}->{'index_entry'}->{'entry_node'} ne $previous_node)) {
      print STDERR "Bug: index entry $current (".
        Texinfo::Convert::Texinfo::convert_to_texinfo(
            {'contents' => $current->{'extra'}->{'index_entry'}->{'entry_content'}})
         .") not in previous node $previous_node\n";
      print STDERR "  previous node: "
        .Texinfo::Convert::Texinfo::root_heading_command_to_texinfo($previous_node)."\n";
      if ($current->{'extra'}->{'index_entry'}->{'entry_node'}) {
        print STDERR "  current node: ".
         Texinfo::Convert::Texinfo::root_heading_command_to_texinfo(
                            $current->{'extra'}->{'index_entry'}->{'entry_node'})."\n";
      } else {
        print STDERR "  current node not set\n";
      }
    }
    $current->{'extra'}->{'index_entry'}->{'entry_node'} = $new_node;
  }
  return ($current);
}

# modifies $nodes_list, $targets_list, $labels
sub insert_nodes_for_sectioning_commands($$$$)
{
  my $root = shift;
  my $nodes_list = shift;
  my $targets_list = shift;
  my $labels = shift;

  my @added_nodes;
  my @contents;
  my $previous_node;
  foreach my $content (@{$root->{'contents'}}) {
    if ($content->{'cmdname'} and $content->{'cmdname'} ne 'node'
        and $content->{'cmdname'} ne 'part'
        and $Texinfo::Commands::root_commands{$content->{'cmdname'}}
        and not ($content->{'extra'}
                 and $content->{'extra'}->{'associated_node'})) {
      my $new_node_tree;
      if ($content->{'cmdname'} eq 'top') {
        $new_node_tree = {'contents' => [{'text' => 'Top'}]};
      } else {
        $new_node_tree = Texinfo::Common::copy_tree({'contents'
          => $content->{'args'}->[0]->{'contents'}});
      }
      my $new_node = _new_node($new_node_tree, $nodes_list,
                               $targets_list, $labels);
      if (defined($new_node)) {
        push @contents, $new_node;
        push @added_nodes, $new_node;
        $new_node->{'extra'}->{'associated_section'} = $content;
        $content->{'extra'}->{'associated_node'} = $new_node;
        $new_node->{'parent'} = $content->{'parent'};
        # reassociate index entries and menus
        Texinfo::Common::modify_tree($content, \&_reassociate_to_node,
                                     [$new_node, $previous_node]);
      }
    }
    # check normalized to avoid erroneous nodes, such as duplicates
    $previous_node = $content
      if ($content->{'cmdname'}
          and $content->{'cmdname'} eq 'node'
          and $content->{'extra'}->{'normalized'});
    push @contents, $content;
  }
  return (\@contents, \@added_nodes);
}

sub _prepend_new_menu_in_node_section($$$)
{
  my $node = shift;
  my $section = shift;
  my $current_menu = shift;

  if (not defined($current_menu)) {
    cluck "input menu undef";
  }
  push @{$section->{'contents'}}, $current_menu;
  push @{$section->{'contents'}}, {'type' => 'empty_line',
                                   'text' => "\n",
                                   'parent' => $section};
  push @{$node->{'extra'}->{'menus'}}, $current_menu;
}

sub complete_node_menu($;$)
{
  my $node = shift;
  my $use_sections = shift;

  my @node_childs
      = Texinfo::Structuring::get_node_node_childs_from_sectioning($node);

  if (scalar(@node_childs)) {
    my %existing_entries;
    if ($node->{'extra'}->{'menus'} and @{$node->{'extra'}->{'menus'}}) {
      foreach my $menu (@{$node->{'extra'}->{'menus'}}) {
        foreach my $entry (@{$menu->{'contents'}}) {
          if ($entry->{'type'} and $entry->{'type'} eq 'menu_entry') {
            my ($normalized_entry_node, $node)
              = _normalized_entry_associated_internal_node($entry);
            if (defined($normalized_entry_node)) {
              $existing_entries{$normalized_entry_node}
                = [$menu, $entry];
            }
          }
        }
      }
    }
    #print STDERR "existing_entries: ".join('|', keys(%existing_entries))."\n";
    my @pending;
    my $current_menu;
    foreach my $node_entry (@node_childs) {
      if ($node_entry->{'extra'}
          and defined($node_entry->{'extra'}->{'normalized'})
          and $existing_entries{$node_entry->{'extra'}->{'normalized'}}) {
        my $entry;
        ($current_menu, $entry)
         = @{$existing_entries{$node_entry->{'extra'}->{'normalized'}}};
        if (@pending) {
          my $index;
          for ($index = 0; $index < scalar(@{$current_menu->{'contents'}}); $index++) {
            #print STDERR "$index, ".scalar(@{$current_menu->{'contents'}})."\n";
            last if ($current_menu->{'contents'}->[$index] eq $entry);
          }
          splice (@{$current_menu->{'contents'}}, $index, 0, @pending);
          foreach my $entry (@pending) {
            $entry->{'parent'} = $current_menu;
          }
          @pending = ();
        }
      } else {
        my $entry = Texinfo::Structuring::new_node_menu_entry($node_entry,
                                                              $use_sections);
        # not defined $entry should mean an empty node.  We do not warn as
        # we try, in general, to be silent in the transformations.
        push @pending, $entry if (defined($entry));
      }
    }
    if (scalar(@pending)) {
      if (!$current_menu) {
        my $section = $node->{'extra'}->{'associated_section'};
        $current_menu =
          Texinfo::Structuring::new_block_command(\@pending, $section, 'menu');
        _prepend_new_menu_in_node_section($node, $section, $current_menu);
      } else {
        foreach my $entry (@pending) {
          $entry->{'parent'} = $current_menu;
        }
        my $end;
        if ($current_menu->{'contents'}->[-1]->{'cmdname'}
            and $current_menu->{'contents'}->[-1]->{'cmdname'} eq 'end') {
          $end = pop @{$current_menu->{'contents'}};
        }
        push @{$current_menu->{'contents'}}, @pending;
        push @{$current_menu->{'contents'}}, $end if ($end);
      }
    }
  }
}

sub _get_non_automatic_nodes_with_sections($)
{
  my $root = shift;

  my @non_automatic_nodes;
  foreach my $content (@{$root->{'contents'}}) {
    if ($content->{'cmdname'} and $content->{'cmdname'} eq 'node'
        and (scalar(@{$content->{'extra'}->{'nodes_manuals'}}) == 1)
        and $content->{'extra'}
        and $content->{'extra'}->{'associated_section'}) {
      push @non_automatic_nodes, $content;
    }
  }
  return [ @non_automatic_nodes ];
}

# This should be called after Texinfo::Structuring::sectioning_structure.
sub complete_tree_nodes_menus($;$)
{
  my $root = shift;
  my $use_sections = shift;

  my $non_automatic_nodes = _get_non_automatic_nodes_with_sections($root);
  foreach my $node (@{$non_automatic_nodes}) {
    complete_node_menu($node, $use_sections);
  }
}

# this only complete menus if there was no menu
sub complete_tree_nodes_missing_menu($;$)
{
  my $root = shift;
  my $use_sections = shift;

  my $non_automatic_nodes = _get_non_automatic_nodes_with_sections($root);
  foreach my $node (@{$non_automatic_nodes}) {
    if (not $node->{'extra'}->{'menus'}
        or not scalar(@{$node->{'extra'}->{'menus'}})) {
      my $section = $node->{'extra'}->{'associated_section'};
      my $current_menu
        = Texinfo::Structuring::new_complete_node_menu($node, $use_sections);
      if (defined($current_menu)) {
        _prepend_new_menu_in_node_section($node, $section, $current_menu);
      }
    }
  }
}

sub _print_down_menus($$);
sub _print_down_menus($$)
{
  my $node = shift;
  my $labels = shift;

  my @master_menu_contents;

  if ($node->{'extra'}->{'menus'} and scalar(@{$node->{'extra'}->{'menus'}})) {
    my @node_children;
    foreach my $menu (@{$node->{'extra'}->{'menus'}}) {
      foreach my $entry (@{$menu->{'contents'}}) {
        if ($entry->{'type'} and $entry->{'type'} eq 'menu_entry') {
          push @master_menu_contents, Texinfo::Common::copy_tree($entry);
          # gather node children to recursively print their menus
          my ($normalized_entry_node, $node)
               = _normalized_entry_associated_internal_node($entry, $labels);
          if (defined($node) and $node->{'extra'}) {
            push @node_children, $node;
          }
        }
      }
    }
    if (scalar(@master_menu_contents)) {
      # Prepend node title
      my $node_title_contents;
      if ($node->{'extra'}->{'associated_section'}
          and $node->{'extra'}->{'associated_section'}->{'args'}
          and $node->{'extra'}->{'associated_section'}->{'args'}->[0]
          and $node->{'extra'}->{'associated_section'}->{'args'}->[0]->{'contents'}) {
        $node_title_contents
          = Texinfo::Common::copy_contents(
                      $node->{'extra'}->{'associated_section'}->{'args'}->[0]->{'contents'});
      } else {
        $node_title_contents = Texinfo::Common::copy_contents($node->{'extra'}->{'node_content'});
      }
      my $menu_comment = {'type' => 'menu_comment', 'contents' => []};
      $menu_comment->{'contents'}->[0] = {'type' => 'preformatted',
                                          'parent' => $menu_comment};
    
      $menu_comment->{'contents'}->[0]->{'contents'}
        = [{'text' => "\n", 'type' => 'empty_line'}, @$node_title_contents,
           {'text' => "\n", 'type' => 'empty_line'},
           {'text' => "\n", 'type' => 'empty_line'}];
      foreach my $content (@{$menu_comment->{'contents'}->[0]->{'contents'}}) {
        $content->{'parent'} = $menu_comment->{'contents'}->[0];
      }
      unshift @master_menu_contents, $menu_comment;

      # now recurse in the children
      foreach my $child (@node_children) {
        push @master_menu_contents, _print_down_menus($child, $labels);
      }
    }
  }
  return @master_menu_contents;
}

if (0) {
  # it is needed to mark the translation as gdt is called like
  # Texinfo::Translations::gdt($self, ' --- The Detailed Node Listing ---')
  # and not like gdt(' --- The Detailed Node Listing ---')
  gdt(' --- The Detailed Node Listing ---');
}

sub _normalized_entry_associated_internal_node($;$)
{
  my $entry = shift;
  my $labels = shift;

  my $entry_node = $entry->{'extra'}->{'menu_entry_node'};
  # 'normalized' is added by Texinfo::Structuring::associate_internal_references
  # so it could be possible not to have 'normalized' set.  In that case, it could
  # still be determined with NodeNameNormalization::normalize_node.
  my $normalized_entry_node = $entry_node->{'normalized'};
  #my $normalized_entry_node
  #  = Texinfo::Convert::NodeNameNormalization::normalize_node(
  #                   {'contents' => $entry_node->{'node_content'}});
  if (! $entry_node->{'manual_content'}
      and defined($normalized_entry_node)) {
    if ($labels) {
      return ($normalized_entry_node, $labels->{$normalized_entry_node});
    } else {
      return ($normalized_entry_node, undef);
    }
  }
  return (undef, undef);
}

sub new_master_menu($$)
{
  my $self = shift;
  my $labels = shift;
  my $node = $labels->{'Top'};
  return undef if (!defined($node));

  my @master_menu_contents;
  if ($node->{'extra'}->{'menus'} and scalar(@{$node->{'extra'}->{'menus'}})) {
    foreach my $menu (@{$node->{'extra'}->{'menus'}}) {
      foreach my $entry (@{$menu->{'contents'}}) {
        if ($entry->{'type'} and $entry->{'type'} eq 'menu_entry') {
          my ($normalized_entry_node, $node)
               = _normalized_entry_associated_internal_node($entry, $labels);
          if (defined($node) and $node->{'extra'}) {
            push @master_menu_contents, _print_down_menus($node, $labels);
          }
        }
      }
    }
  }
  if (scalar(@master_menu_contents)) {
    my $first_preformatted = $master_menu_contents[0]->{'contents'}->[0];
    my $master_menu_title = Texinfo::Translations::gdt($self,
                                      ' --- The Detailed Node Listing ---');
    my @master_menu_title_contents;
    foreach my $content (@{$master_menu_title->{'contents'}}, {'text' => "\n"}) {
      $content->{'parent'} = $first_preformatted;
      push @master_menu_title_contents, $content;
    }
    unshift @{$first_preformatted->{'contents'}}, @master_menu_title_contents;
    return Texinfo::Structuring::new_block_command(\@master_menu_contents, undef,
                                                   'detailmenu');
  } else {
    return undef;
  }
}

# self is used to pass down a translatable object with customization
# information for the gdt() call.
sub regenerate_master_menu($$)
{
  my $self = shift;
  my $labels = shift;
  my $top_node = $labels->{'Top'};
  return undef if (!defined($top_node));

  my $new_master_menu = new_master_menu($self, $labels);
  return undef if (!defined($new_master_menu) or !$top_node->{'extra'}->{'menus'}
                   or !scalar(@{$top_node->{'extra'}->{'menus'}}));

  foreach my $menu (@{$top_node->{'extra'}->{'menus'}}) {
    my $detailmenu_index = 0;
    foreach my $entry (@{$menu->{'contents'}}) {
      if ($entry->{'cmdname'} and $entry->{'cmdname'} eq 'detailmenu') {
        # replace existing detailmenu by the master menu
        $new_master_menu->{'parent'} = $menu;
        splice (@{$menu->{'contents'}}, $detailmenu_index, 1,
                $new_master_menu);
        return 1;
      }
      $detailmenu_index++;
    }
  }

  my $last_menu = $top_node->{'extra'}->{'menus'}->[-1];
  my $index = scalar(@{$last_menu->{'contents'}});
  if ($index
      and $last_menu->{'contents'}->[$index-1]->{'cmdname'}
      and $last_menu->{'contents'}->[$index-1]->{'cmdname'} eq 'end') {
    $index --;
  }

  $new_master_menu->{'parent'} = $last_menu;
  if ($index
      and $last_menu->{'contents'}->[$index-1]->{'type'}
      and $last_menu->{'contents'}->[$index-1]->{'type'} eq 'menu_comment'
      and $last_menu->{'contents'}->[$index-1]->{'contents'}->[-1]->{'type'}
      and $last_menu->{'contents'}->[$index-1]->{'contents'}->[-1]->{'type'}
             eq 'preformatted') {
    # there is already a menu comment at the end of the menu, add an empty line
    my $empty_line = {'type' => 'empty_line', 'text' => "\n", 'parent' =>
               $last_menu->{'contents'}->[$index-1]->{'contents'}->[-1]};
    push @{$last_menu->{'contents'}->[$index-1]->{'contents'}}, $empty_line;
  } elsif ($index
           and $last_menu->{'contents'}->[$index-1]->{'type'}
           and $last_menu->{'contents'}->[$index-1]->{'type'} eq 'menu_entry') {
    # there is a last menu entry, add a menu comment containing an empty line
    # after it
    my $menu_comment = {'type' => 'menu_comment', 'parent' => $last_menu};
    splice (@{$last_menu->{'contents'}}, $index, 0, $menu_comment);
    $index++;
    my $preformatted = {'type' => 'preformatted', 'parent' => $menu_comment};
    push @{$menu_comment->{'contents'}}, $preformatted;
    my $empty_line = {'type' => 'after_menu_description_line', 'text' => "\n",
                      'parent' => $preformatted};
    push @{$preformatted->{'contents'}}, $empty_line;
  }
  # insert master menu
  splice (@{$last_menu->{'contents'}}, $index, 0, $new_master_menu);

  return 1;
}

# modify the menu tree to put description and menu comment content
# together directly in the menu.  Put the menu_entry in a preformatted.
# last merge preformatted.
sub menu_to_simple_menu($);

sub menu_to_simple_menu($)
{
  my $menu = shift;
  
  my @contents;
  foreach my $content (@{$menu->{'contents'}}) {
    if ($content->{'type'} and $content->{'type'} eq 'menu_comment') {
      push @contents, @{$content->{'contents'}};
    } elsif ($content->{'type'} and $content->{'type'} eq 'menu_entry') {
      my $preformatted = {'type' => 'preformatted', 'contents' => [$content]};
      push @contents, $preformatted;
      $content->{'parent'} = $preformatted;

      my $in_description;
      my @args = @{$content->{'args'}};
      @{$content->{'args'}} = ();
      while (@args) {
        if ($args[0]->{'type'} and $args[0]->{'type'} eq 'menu_entry_description') {
          my $description = shift @args;
          push @contents, @{$description->{'contents'}};
          push @contents, @args;
          last;
        } else {
          my $arg = shift @args;
          push @{$content->{'args'}}, $arg;
        }
      }
    } elsif ($content->{'cmdname'}
             and $Texinfo::Commands::block_commands{$content->{'cmdname'}}
  and $Texinfo::Commands::block_commands{$content->{'cmdname'}} eq 'menu') {
      menu_to_simple_menu($content);
      push @contents, $content;
    } else {
      push @contents, $content;
    }
  }
  
  # reset parent, put in menu and merge preformatted.
  @{$menu->{'contents'}} = ();
  my $current_preformatted;
  foreach my $content (@contents) {
    $content->{'parent'} = $menu;
    if ($content->{'type'} and $content->{'type'} eq 'preformatted') {
      if (!defined($current_preformatted)) {
        $current_preformatted = $content;
        push @{$menu->{'contents'}}, $content;
      } else {
        foreach my $preformatted_content (@{$content->{'contents'}}) {
          push @{$current_preformatted->{'contents'}}, $preformatted_content;
          $preformatted_content->{'parent'} = $current_preformatted;
        }
      }
    } else {
      $current_preformatted = undef;
      push @{$menu->{'contents'}}, $content;
    }
  }
}

sub set_menus_to_simple_menu($)
{
  my $nodes_list = shift;

  if ($nodes_list) {
    foreach my $node (@{$nodes_list}) {
      if ($node->{'extra'}->{'menus'}) {
        foreach my $menu (@{$node->{'extra'}->{'menus'}}) {
          menu_to_simple_menu($menu);
        }
      }
    }
  }
}

sub _is_cpp_line($)
{
  my $text = shift;
  return 1 if ($text =~ /^\s*#\s*(line)? (\d+)(( "([^"]+)")(\s+\d+)*)?\s*$/);
  return 0;
}

# An element can be marked to be protected, it will actually be protected
# when it is processed later on by Texinfo::Common::modify_tree
sub _protect_hashchar_at_line_beginning($$$)
{
  my $type = shift;
  my $current = shift;
  my $argument = shift;

  my ($registrar, $customization_information, $elements_to_protect) = @$argument;

  #print STDERR "$type $current ".debug_print_element($current)."\n";
  # if the next is a hash character at line beginning, mark it
  if (defined($current->{'text'}) and $current->{'text'} =~ /\n$/
      and $current->{'parent'} and $current->{'parent'}->{'contents'}) {
    my $parent = $current->{'parent'};
    #print STDERR "End of line in $current, parent $parent: (@{$parent->{'contents'}})\n";
    my $current_found = 0;
    foreach my $content (@{$parent->{'contents'}}) {
      if ($current_found) {
        #print STDERR "after $current: $content $content->{'text'}\n";
        if ($content->{'text'} and _is_cpp_line($content->{'text'})) {
          $elements_to_protect->{$content} = 1;
        }
        last;
      } elsif ($content eq $current) {
        $current_found = 1;
      }
    }
  }

  my $protect_hash = 0;
  # if marked, or first and a cpp_line protect a leading hash character
  if ($elements_to_protect->{$current}) {
    $protect_hash = 1;
    delete $elements_to_protect->{$current};
  } elsif ($current->{'parent'}
           and $current->{'parent'}->{'contents'}
           and $current->{'parent'}->{'contents'}->[0]
           and $current->{'parent'}->{'contents'}->[0] eq $current
           and $current->{'text'}
           and _is_cpp_line($current->{'text'})) {
    $protect_hash = 1;
  }
  if ($protect_hash) {
    my @result = ();
    if ($current->{'type'} and $current->{'type'} eq 'raw') {
      my $parent = $current->{'parent'};
      while ($parent) {
        if ($parent->{'cmdname'} and $parent->{'source_info'}) {
          if ($registrar) {
            $registrar->line_warn($customization_information, sprintf(__(
                "could not protect hash character in \@%s"),
                           $parent->{'cmdname'}), $parent->{'source_info'});
          }
          last;
        }
        $parent = $parent->{'parent'};
      }
    } else {
      $current->{'text'} =~ s/^(\s*)#//;
      if ($1 ne '') {
        push @result, {'text' => $1, 'parent' => $current->{'parent'}};
      }
      push @result, {'cmdname' => 'hashchar', 'parent' => $current->{'parent'},
                     'args' => [{'type' => 'brace_command_arg'}]};
    }
    push @result, $current;
    return @result;
  } else {
    return ($current);
  }
}

sub protect_hashchar_at_line_beginning($$$)
{
  my $registrar = shift;
  my $customization_information = shift;
  my $tree = shift;

  my $elements_to_protect = {};
  return Texinfo::Common::modify_tree($tree, \&_protect_hashchar_at_line_beginning,
                      [$registrar, $customization_information, $elements_to_protect]);
}

1;

__END__

=head1 NAME

Texinfo::Transformations - transformations of Texinfo Perl tree

=head1 NOTES

The Texinfo Perl module main purpose is to be used in C<texi2any> to convert
Texinfo to other formats.  There is no promise of API stability.

=head1 DESCRIPTION

Includes miscellaneous methods C<set_menus_to_simple_menu> and
C<menu_to_simple_menu> to change the menu texinfo tree, as well
as C<insert_nodes_for_sectioning_commands> that adds nodes for
sectioning commands without nodes and C<complete_tree_nodes_menus>
and C<complete_tree_nodes_missing_menu> that completes the
node menus based on the sectioning tree.

=head1 METHODS

No method is exported in the default case.

=over

=item complete_tree_nodes_menus($tree, $add_section_names_in_entries)
X<C<complete_tree_nodes_menus>>

Add menu entries or whole menus for nodes associated with sections,
based on the sectioning tree.  If the optional
C<$add_section_names_in_entries> argument is set, a menu entry
name is added using the section name.  This function should be
called after L<sectioning_structure|Texinfo::Structuring/$sections_root, $sections_list = sectioning_structure($registrar, $customization_information, $tree)>.

=item complete_tree_nodes_missing_menu($tree, $use_section_names_in_entries)
X<C<complete_tree_nodes_missing_menu>>

Add whole menus for nodes associated with sections and without menu,
based on the sectioning tree.  If the optional
C<$add_section_names_in_entries> argument is set, a menu entry
name is added using the section name.  This function should be
called after L<sectioning_structure|Texinfo::Structuring/$sections_root, $sections_list = sectioning_structure($registrar, $customization_information, $tree)>.

=item ($root_content, $added_sections) = fill_gaps_in_sectioning($tree)
X<C<fill_gaps_in_sectioning>>

This function adds empty C<@unnumbered> and similar commands in a tree
to fill gaps in sectioning.  This may be used, for example, when converting
from a format that can handle gaps in sectioning.  I<$tree> is the tree
root.  An array reference is returned, containing the root contents
with added sectioning commands, as well as an array reference containing
the added sectioning commands.

If the sectioning commands are lowered or raised (with C<@raisesections>,
C<@lowersection>) the tree may be modified with C<@raisesections> or
C<@lowersection> added to some tree elements.

=item ($root_content, $added_nodes) = insert_nodes_for_sectioning_commands($tree, $nodes_list, $targets_list, $labels)
X<C<insert_nodes_for_sectioning_commands>>

Insert nodes for sectioning commands without node in C<$tree>.
Add nodes to the labels used as targets for references C<$labels>
and C<$targets_list> and to C<$nodes_list>.

An array reference is returned, containing the root contents
with added nodes, as well as an array reference containing the
added nodes.

=item menu_to_simple_menu($menu)

=item set_menus_to_simple_menu($nodes_list)
X<C<menu_to_simple_menu>>
X<C<set_menus_to_simple_menu>>

C<menu_to_simple_menu> transforms the tree of a menu tree element.
C<set_menus_to_simple_menu> calls C<menu_to_simple_menu> for all the
menus of the nodes in C<$nodes_list>.

A simple menu has no I<menu_comment>, I<menu_entry> or I<menu_entry_description>
container anymore, their content are merged directly in the menu in
I<preformatted> container.

=item $detailmenu = new_master_menu($translations, $labels)
X<C<new_master_menu>>

Returns a detailmenu tree element formatted as a master node.
I<$translations>, if defined, should be a L<Texinfo::Translations> object and
should also hold customization information.

=item protect_hashchar_at_line_beginning($registrar, $customization_information, $tree)
X<C<protect_hashchar_at_line_beginning>>

Protect hash (#) character at the beginning of line such that they would not be
considered as lines to be processed by the CPP processor.  The I<$registrar>
and I<$customization_information> arguments may be undef.  If defined, the
I<$registrar> argument should be a L<Texinfo::Report> object in which the
errors and warnings encountered while parsing are registered.  If defined,
I<$customization_information> should give access to customization through
C<get_conf>.  If both I<$registrar> and I<$customization_information> are
defined they are used for error reporting in case an hash character could not
be protected because it appeared in a raw environment.

=item $modified_tree = reference_to_arg_in_tree($tree)
X<C<reference_to_arg_in_tree>>

Modify I<$tree> by converting reference @-commands to simple text using one of
the arguments.  This transformation can be used, for example, to remove
reference @-command from constructed node names trees, as node names cannot
contain reference @-command while there could be some in the tree used in input
for the node name tree.

=item regenerate_master_menu($translations, $labels)
X<C<regenerate_master_menu>>

Regenerate the Top node master menu, replacing the first detailmenu
in Top node menus or appending at the end of the Top node menu.
I<$translations>, if defined, should be a L<Texinfo::Translations> object and
should also hold customization information.

=back

=head1 SEE ALSO

L<Texinfo manual|http://www.gnu.org/s/texinfo/manual/texinfo/>,
L<Texinfo::Parser>.

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
