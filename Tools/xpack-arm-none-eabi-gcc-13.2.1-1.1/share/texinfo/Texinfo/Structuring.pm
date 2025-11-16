# Structuring.pm: extract information about a document structure based on the
#                 document tree.
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

package Texinfo::Structuring;

use 5.00405;

# See comment at start of HTML.pm
use if $] >= 5.012, feature => 'unicode_strings';

use strict;
# Can be used to check that there is no incorrect autovivfication
# In that case, creations of hashes should be uncommented
#no autovivification qw(fetch delete exists store strict);

use Carp qw(cluck confess);

# for %root_commands
use Texinfo::Commands;
use Texinfo::Common;

# for error messages
use Texinfo::Convert::Texinfo qw(node_extra_to_texi);
# for debugging.  Also for index entries sorting.
use Texinfo::Convert::Text;
# for internal references and misc uses
use Texinfo::Convert::NodeNameNormalization;


require Exporter;
use vars qw($VERSION @ISA @EXPORT_OK %EXPORT_TAGS);
@ISA = qw(Exporter);

%EXPORT_TAGS = ( 'all' => [ qw(
  associate_internal_references
  check_nodes_are_referenced
  complete_node_tree_with_menus
  elements_directions
  elements_file_directions
  merge_indices
  nodes_tree
  number_floats
  sectioning_structure
  set_menus_node_directions
  sort_indices
  split_by_node
  split_by_section
  split_pages
  warn_non_empty_parts
) ] );

@EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

$VERSION = '7.0.3';


my %types_to_enter;
foreach my $type_to_enter ('brace_command_arg', 'line_arg',
    'paragraph') {
  $types_to_enter{$type_to_enter} = 1;
}


my %command_structuring_level = %Texinfo::Common::command_structuring_level;

my %appendix_commands;
my %unnumbered_commands;
foreach my $command (keys(%command_structuring_level)) {
  if ($command =~ /appendix/) {
    $appendix_commands{$command} = 1;
  } elsif ($command =~ /unnumbered/) {
    $unnumbered_commands{$command} = 1;
  }
}
$unnumbered_commands{'top'} = 1;
$unnumbered_commands{'centerchap'} = 1;
$unnumbered_commands{'part'} = 1;

# Go through the sectioning commands (e.g. @chapter, not @node), and
# set:
# 'section_level'
# 'section_number'
# 'section_childs'
# 'section_up'
# 'section_prev'
# 'section_next'
# 'toplevel_next'
# 'toplevel_prev'
# 'toplevel_up'
sub sectioning_structure($$$)
{
  my $registrar = shift;
  my $customization_information = shift;
  my $root = shift;

  my $sec_root;
  my $previous_section;
  my $previous_toplevel;

  my $in_appendix = 0;
  # lowest level with a number.  This is the lowest level above 0.
  my $number_top_level;

  my $section_top;
  my @sections_list;
  
  # holds the current number for all the levels.  It is not possible to use
  # something like the last child index, because of @unnumbered.
  my @command_numbers;
  # keep track of the unnumbered
  my @command_unnumbered;
  foreach my $content (@{$root->{'contents'}}) {
    if (!$content->{'cmdname'} or $content->{'cmdname'} eq 'node'
        or $content->{'cmdname'} eq 'bye') {
      next;
    }
    push @sections_list, $content;
    if ($content->{'cmdname'} eq 'top') {
      if (! $section_top) {
        $section_top = $content;
      }
    }
    my $level;
    #$content->{'structure'} = {} if (! $content->{'structure'});
    $level = $content->{'structure'}->{'section_level'}
         = Texinfo::Common::section_level($content);
    if (!defined($level)) {
      warn "bug: level not defined for $content->{'cmdname'}\n";
      $level = $content->{'structure'}->{'section_level'} = 0;
    }

    if ($previous_section) {
      # new command is below
      if ($previous_section->{'structure'}->{'section_level'} < $level) {
        if ($level - $previous_section->{'structure'}->{'section_level'} > 1) {
          $registrar->line_error($customization_information,
           sprintf(__("raising the section level of \@%s which is too low"),
              $content->{'cmdname'}), $content->{'source_info'});
          $content->{'structure'}->{'section_level'}
              = $previous_section->{'structure'}->{'section_level'} + 1;
        }
        $previous_section->{'structure'}->{'section_childs'} = [$content];
        $content->{'structure'}->{'section_up'} = $previous_section;

        # if the up is unnumbered, the number information has to be kept,
        # to avoid reusing an already used number.
        if (!$unnumbered_commands{$previous_section->{'cmdname'}}) {
          $command_numbers[$content->{'structure'}->{'section_level'}] = undef;
        } elsif (!$unnumbered_commands{$content->{'cmdname'}}) {
          $command_numbers[$content->{'structure'}->{'section_level'}]++;
        }
        if ($unnumbered_commands{$content->{'cmdname'}}) {
          $command_unnumbered[$content->{'structure'}->{'section_level'}] = 1;
        } else {
          $command_unnumbered[$content->{'structure'}->{'section_level'}] = 0;
        }
      } else {
        my $up = $previous_section->{'structure'}->{'section_up'};
        my $new_upper_part_element;
        if ($previous_section->{'structure'}->{'section_level'} != $level) {
          # means it is above the previous command, the up is to be found
          while ($up->{'structure'}->{'section_up'}
                 and $up->{'structure'}->{'section_level'} >= $level) {
            $up = $up->{'structure'}->{'section_up'};
          }
          if ($level <= $up->{'structure'}->{'section_level'}) {
            if ($content->{'cmdname'} eq 'part') {
              $new_upper_part_element = 1;
              if ($level < $up->{'structure'}->{'section_level'}) {
                $registrar->line_warn($customization_information,
                 sprintf(__("no chapter-level command before \@%s"),
                    $content->{'cmdname'}), $content->{'source_info'});
              }
            } else {
              $registrar->line_warn($customization_information,
  sprintf(__("lowering the section level of \@%s appearing after a lower element"),
                  $content->{'cmdname'}), $content->{'source_info'});
              $content->{'structure'}->{'section_level'}
                 = $up->{'structure'}->{'section_level'} + 1;
            }
          }
        }
        if ($appendix_commands{$content->{'cmdname'}} and !$in_appendix
            and $content->{'structure'}->{'section_level'} <= $number_top_level
            and $up->{'cmdname'} and $up->{'cmdname'} eq 'part') {
          $up = $up->{'structure'}->{'section_up'};
        }
        if ($new_upper_part_element) {
          # In that case the root has to be updated because the first
          # 'part' just appeared
          $sec_root->{'structure'}->{'section_level'} = $level - 1;
          push @{$sec_root->{'structure'}->{'section_childs'}}, $content;
          $content->{'structure'}->{'section_up'} = $sec_root;
          $number_top_level = $level;
          $number_top_level++ if (!$number_top_level);
        } else {
          push @{$up->{'structure'}->{'section_childs'}}, $content;
          $content->{'structure'}->{'section_up'} = $up;
          $content->{'structure'}->{'section_prev'} = $up->{'structure'}->{'section_childs'}->[-2];
          $content->{'structure'}->{'section_prev'}->{'structure'}->{'section_next'} = $content;
        }
        if (!$unnumbered_commands{$content->{'cmdname'}}) {
          $command_numbers[$content->{'structure'}->{'section_level'}]++;
          $command_unnumbered[$content->{'structure'}->{'section_level'}] = 0;
        } else {
          $command_unnumbered[$content->{'structure'}->{'section_level'}] = 1;
        }
      }
    } else {
      $sec_root = {'structure' => {}};
      # first section determines the level of the root.  It is
      # typically -1 when there is a @top.
      $sec_root->{'structure'}->{'section_level'} = $level - 1;
      $sec_root->{'structure'}->{'section_childs'} = [$content];
      $content->{'structure'}->{'section_up'} = $sec_root;
      $number_top_level = $level;
      # if $level of top sectioning element is 0, which means that
      # it is a @top, $number_top_level is 1 as it is associated to
      # the level of chapter/unnumbered...
      $number_top_level++ if (!$number_top_level);
      if ($content->{'cmdname'} ne 'top') {
        if (!$unnumbered_commands{$content->{'cmdname'}}) {
          $command_unnumbered[$content->{'structure'}->{'section_level'}] = 0;
        } else {
          $command_unnumbered[$content->{'structure'}->{'section_level'}] = 1;
        }
      }
    }
    if (!defined($command_numbers[$content->{'structure'}->{'section_level'}])) {
      if ($unnumbered_commands{$content->{'cmdname'}}) {
        $command_numbers[$content->{'structure'}->{'section_level'}] = 0;
      } else {
        $command_numbers[$content->{'structure'}->{'section_level'}] = 1;
      }
    }
    if ($appendix_commands{$content->{'cmdname'}} and !$in_appendix) {
      $in_appendix = 1;
      $command_numbers[$content->{'structure'}->{'section_level'}] = 'A';
    }
    if (!$unnumbered_commands{$content->{'cmdname'}}) {
      # construct the number, if not below an unnumbered
      if (!$command_unnumbered[$number_top_level]) {
        my $section_number = $command_numbers[$number_top_level];
        for (my $i = $number_top_level+1; $i <= $content->{'structure'}->{'section_level'}; $i++) {
          $section_number .= ".$command_numbers[$i]";
          # If there is an unnumbered above, then no number is added.
          if ($command_unnumbered[$i]) {
            $section_number = undef;
            last;
          }
        }
        if (defined($section_number)) {
          $content->{'structure'}->{'section_number'} = $section_number;
        }
      }
    }
    $previous_section = $content;
    if ($content->{'cmdname'} ne 'part'
        and $content->{'structure'}->{'section_level'} <= $number_top_level) {
      if ($previous_toplevel) {
        $previous_toplevel->{'structure'}->{'toplevel_next'} = $content;
        $content->{'structure'}->{'toplevel_prev'} = $previous_toplevel;
      }
      $previous_toplevel = $content;
      if ($section_top and $content ne $section_top) {
        $content->{'structure'}->{'toplevel_up'} = $section_top;
      }
    } elsif ($content->{'cmdname'} eq 'part'
             and !$content->{'extra'}->{'part_associated_section'}) {
      $registrar->line_warn($customization_information,
        sprintf(__("no sectioning command associated with \@%s"),
          $content->{'cmdname'}), $content->{'source_info'});
    }
  }
  return $sec_root, \@sections_list;
}

# for debugging
sub _print_sectioning_tree($);
sub _print_sectioning_tree($)
{
  my $current = shift;
  my $result = ' ' x $current->{'structure'}->{'section_level'}
     . Texinfo::Convert::Texinfo::root_heading_command_to_texinfo($current)."\n";
  foreach my $child (@{$current->{'structure'}->{'section_childs'}}) {
    $result .= _print_sectioning_tree($child);
  }
  return $result;
}


sub warn_non_empty_parts($$$)
{
  my $registrar = shift;
  my $customization_information = shift;
  my $global_commands = shift;

  if ($global_commands->{'part'}) {
    foreach my $part (@{$global_commands->{'part'}}) {
      if (!Texinfo::Common::is_content_empty($part)) {
        $registrar->line_warn($customization_information,
               sprintf(__("\@%s not empty"),
                       $part->{'cmdname'}), $part->{'source_info'});
      }
    }
  }
}

sub _check_node_same_texinfo_code($$)
{
  my $reference_node = shift;
  my $node_extra = shift;

  my $reference_node_texi;
  if ($reference_node->{'extra'}->{'node_content'}) {
    $reference_node_texi = Texinfo::Convert::Texinfo::convert_to_texinfo(
        {'contents' => $reference_node->{'extra'}->{'node_content'}});
    $reference_node_texi =~ s/\s+/ /g;
  } else {
    $reference_node_texi = '';
  }

  my $node_texi;
  if ($node_extra and $node_extra->{'node_content'}) {
    my @contents_node = @{$node_extra->{'node_content'}};
    pop @contents_node if ($contents_node[-1]->{'type'}
               and $contents_node[-1]->{'type'} eq 'space_at_end_menu_node');
    $node_texi = Texinfo::Convert::Texinfo::convert_to_texinfo(
      {'contents' => \@contents_node});
    $node_texi =~ s/\s+/ /g;
  } else {
    $node_texi = '';
  }
  return ($reference_node_texi eq $node_texi);
}


my @node_directions = ('next', 'prev', 'up');
# No translation of those special Info keywords.
my %direction_texts = (
 'prev' => 'Prev',
 'next' => 'Next',
 'up' => 'Up'
);

sub _check_menu_entry($$$$$)
{
  my $registrar = shift;
  my $customization_information = shift;
  my $labels = shift;
  my $command = shift;
  my $menu_content = shift;

  my $normalized_menu_node
      = $menu_content->{'extra'}->{'menu_entry_node'}->{'normalized'};

  my $menu_node = $labels->{$normalized_menu_node};

  if (!$menu_node) {
    $registrar->line_error($customization_information,
      sprintf(__("\@%s reference to nonexistent node `%s'"), $command,
           node_extra_to_texi($menu_content->{'extra'}->{'menu_entry_node'})),
      $menu_content->{'source_info'});
  } else {
    if (!_check_node_same_texinfo_code($menu_node,
                           $menu_content->{'extra'}->{'menu_entry_node'})) {
      $registrar->line_warn($customization_information,
       sprintf(__("\@%s entry node name `%s' different from %s name `%s'"),
         $command,
         node_extra_to_texi($menu_content->{'extra'}->{'menu_entry_node'}),
         $menu_node->{'cmdname'},
         node_extra_to_texi($menu_node->{'extra'})),
       $menu_content->{'source_info'});
    }
  }
}

# In general should be called only after complete_node_tree_with_menus
# to try to generate menus automatically before checking.
sub check_nodes_are_referenced
{
  my ($registrar, $customization_information, $nodes_list, $top_node, $labels, $refs) = @_;

  return undef unless ($nodes_list and scalar(@{$nodes_list}));

  my %referenced_nodes = ($top_node => 1);
  foreach my $node (@{$nodes_list}) {
    next if (!$node->{'structure'});
    # gather referenced nodes based on node pointers
    foreach my $direction (@node_directions) {
      if ($node->{'structure'}->{'node_'.$direction}
          and not $node->{'structure'}->{'node_'.$direction}->{'extra'}->{'manual_content'}) {
        $referenced_nodes{$node->{'structure'}->{'node_'.$direction}} = 1;
      }
    }
    if ($node->{'structure'}->{'menu_up_hash'}) {
      $referenced_nodes{$node} = 1;
    }
  }

  # consider nodes in @*ref commands to be referenced
  if (defined($refs)) {
    foreach my $ref (@$refs) {
      my $node_arg = $ref->{'extra'}->{'node_argument'};
      if ($node_arg and $node_arg->{'node_content'}) {
        my $normalized =
           Texinfo::Convert::NodeNameNormalization::normalize_node(
              {'contents' => $node_arg->{'node_content'} });
        my $node_target = $labels->{$normalized};
        if ($node_target) {
          $referenced_nodes{$node_target} = 1;
        }
      }
    }
  }

  foreach my $node (@{$nodes_list}) {
    if (not exists($referenced_nodes{$node})) {
      $registrar->line_warn($customization_information,
                            sprintf(__("node `%s' unreferenced"),
                                    node_extra_to_texi($node->{'extra'})),
                            $node->{'source_info'});
    }
  }
}

# set menu directions
sub set_menus_node_directions($$$$$$)
{
  my $registrar = shift;
  my $customization_information = shift;
  my $parser_information = shift;
  my $global_commands = shift;
  my $nodes_list = shift;
  my $labels = shift;

  return undef unless ($nodes_list and scalar(@{$nodes_list}));

  my $check_menu_entries = (!$customization_information->get_conf('novalidate')
      and $customization_information->get_conf('FORMAT_MENU') eq 'menu');

  # First go through all the menus and set menu_up, menu_next and menu_prev,
  # and warn for unknown nodes.
  # Remark: since the @menu are only checked if they are in @node,
  # menu entries before the first node, or @menu nested inside
  # another command such as @format, may be treated slightly
  # differently; at least, there are no error messages for them.
  #
  foreach my $node (@{$nodes_list}) {
    if ($node->{'extra'}->{'menus'}) {
      if (@{$node->{'extra'}->{'menus'}} > 1) {
        foreach my $menu (@{$node->{'extra'}->{'menus'}}[1 .. $#{$node->{'extra'}->{'menus'}}]) {
          $registrar->line_warn($customization_information,
                 sprintf(__("multiple \@%s"),
                         $menu->{'cmdname'}), $menu->{'source_info'});
        }
      }
      foreach my $menu (@{$node->{'extra'}->{'menus'}}) {
        my $previous_node;
        my $menu_contents = $menu;
        # simple menu
        if (scalar(@{$menu->{'contents'}}) > 0
            and $menu->{'contents'}->[0]->{'type'}
            and $menu->{'contents'}->[0]->{'type'} eq 'preformatted') {
          $menu_contents = $menu->{'contents'}->[0];
        }
        foreach my $menu_content (@{$menu_contents->{'contents'}}) {
          if ($menu_content->{'extra'}
             and $menu_content->{'extra'}->{'menu_entry_node'}) {
            my $menu_node;
            my $external_node;
            if (!$menu_content->{'extra'}->{'menu_entry_node'}->{'manual_content'}) {
              $menu_node = $labels->{
                      $menu_content->{'extra'}
                                   ->{'menu_entry_node'}->{'normalized'}};

              if ($check_menu_entries) {
                _check_menu_entry($registrar, $customization_information,
                                  $labels, 'menu', $menu_content);
              }
              # this may happen more than once for a given node if the node
              # is in more than one menu.  Therefore all the menu up node
              # are kept in $menu_node->{'structure'}->{'menu_up_hash'}
              if ($menu_node) {
                #$menu_node->{'structure'} = {} if (!$menu_node->{'structure'});
                $menu_node->{'structure'}->{'menu_up'} = $node;
                #$menu_node->{'structure'}->{'menu_up_hash'} = {} if (!$menu_node->{'structure'}->{'menu_up_hash'});
                $menu_node->{'structure'}->{'menu_up_hash'}->{$node->{'extra'}->{'normalized'}} = 1;
              }
            } else {
              $external_node = 1;
              $menu_node = {'extra' => $menu_content->{'extra'}->{'menu_entry_node'}};
            }
            if ($menu_node) {
              if ($previous_node) {
                if (!$external_node) {
                  #$menu_node->{'structure'} = {} if (!$menu_node->{'structure'});
                  $menu_node->{'structure'}->{'menu_prev'} = $previous_node;
                }
                if (!$previous_node->{'extra'}->{'manual_content'}) {
                  #$previous_node->{'structure'} = {} if (!$previous_node->{'structure'});
                  $previous_node->{'structure'}->{'menu_next'} = $menu_node;
                }
              } else {
                #$node->{'structure'} = {} if (!$node->{'structure'});
                $node->{'structure'}->{'menu_child'} = $menu_node;
              }
              $previous_node = $menu_node;
            }
          }
        } # end menu
      }
    }
  }
  # Check @detailmenu
  if ($check_menu_entries) {
    if ($global_commands->{'detailmenu'}) {
      foreach my $detailmenu (@{$global_commands->{'detailmenu'}}) {
        foreach my $menu_content (@{$detailmenu->{'contents'}}) {
          if ($menu_content->{'extra'}
             and $menu_content->{'extra'}->{'menu_entry_node'}) {
            if (!$menu_content->{'extra'}->{'menu_entry_node'}->{'manual_content'}) {
              _check_menu_entry($registrar, $customization_information,
                                $labels, 'detailmenu', $menu_content);
            }
          }
        }
      }
    }
  }
}

# determine node found through section directions, usually
# from section_$direction.  It could also be from
# toplevel_$direction if going through parts, except for @top
# as prev or next.
sub _section_direction_associated_node($$)
{
  my $section = shift;
  my $direction = shift;

  foreach my $direction_base ('section', 'toplevel') {
    if ($section->{'structure'}->{$direction_base.'_'.$direction}
       and $section->{'structure'}->{$direction_base.'_'.$direction}->{'extra'}
       and ($direction_base ne 'toplevel'
            or $direction eq 'up'
            or $section->{'structure'}->{$direction_base.'_'.$direction}->{'cmdname'} ne 'top')
       and $section->{'structure'}->{$direction_base.'_'.$direction}->{'extra'}->{'associated_node'}) {
         return $section->{'structure'}->{$direction_base.'_'.$direction}->{'extra'}->{'associated_node'};
    }
  }
  return undef;
}

# complete automatic directions with menus (and first node
# for Top node).
# Checks on structure related to menus.
sub complete_node_tree_with_menus($$$$)
{
  my $registrar = shift;
  my $customization_information = shift;
  my $nodes_list = shift;
  my $top_node = shift;

  return undef unless ($nodes_list and @{$nodes_list});
  # Go through all the nodes
  foreach my $node (@{$nodes_list}) {
    my $automatic_directions =
      (scalar(@{$node->{'extra'}->{'nodes_manuals'}}) == 1);

    if ($automatic_directions) {
      if ($node->{'extra'}->{'normalized'} ne 'Top') {
        foreach my $direction (@node_directions) {
          # prev already defined for the node first Top node menu entry
          if ($direction eq 'prev' and $node->{'structure'}
              and $node->{'structure'}->{'node_'.$direction}
              and $node->{'structure'}->{'node_'.$direction}->{'extra'}
              and $node->{'structure'}->{'node_'.$direction}->{'extra'}->{'normalized'}
              and $node->{'structure'}->{'node_'.$direction}->{'extra'}->{'normalized'} eq 'Top') {
            next;
          }
          if ($node->{'extra'}->{'associated_section'}) {
            my $section = $node->{'extra'}->{'associated_section'};

            # Prefer the section associated with a @part for node directions.
            if ($section->{'extra'}->{'part_associated_section'}) {
              $section = $section->{'extra'}->{'part_associated_section'};
            }
            # Check consistency with section and menu structure
            my $direction_associated_node
              = _section_direction_associated_node($section, $direction);
            if ($direction_associated_node) {
              if ($customization_information->get_conf('CHECK_NORMAL_MENU_STRUCTURE')) {
                if ($section->{'structure'}->{'section_up'}{'extra'}
          and $section->{'structure'}->{'section_up'}{'extra'}{'associated_node'}
          and $section->{'structure'}->{'section_up'}{'extra'}{'associated_node'}{'menus'}
          and @{$section->{'structure'}->{'section_up'}{'extra'}{'associated_node'}{'menus'}}
                    and !$node->{'structure'}->{'menu_'.$direction}) {
                  $registrar->line_warn($customization_information,
           sprintf(__("node %s for `%s' is `%s' in sectioning but not in menu"),
                   $direction,
                   node_extra_to_texi($node->{'extra'}),
                   node_extra_to_texi($direction_associated_node->{'extra'})),
                                        $node->{'source_info'});
                }
              }
            }
          }
          # no direction was found using sections, use menus.  This allows
          # using only automatic direction for manuals without sectioning
          # commands.
          if ($node->{'structure'}
              and !$node->{'structure'}->{'node_'.$direction}
              and $node->{'structure'}->{'menu_'.$direction}
              and !$node->{'structure'}->{'menu_'.$direction}->{'extra'}->{'manual_content'}) {
            if ($customization_information->get_conf('CHECK_NORMAL_MENU_STRUCTURE')
                  and $node->{'extra'}->{'associated_section'}) {
              $registrar->line_warn($customization_information,
          sprintf(__("node `%s' is %s for `%s' in menu but not in sectioning"),
                node_extra_to_texi($node->{'structure'}->{'menu_'.$direction}->{'extra'}),
                                   $direction,
                node_extra_to_texi($node->{'extra'}),
                  ),
                $node->{'source_info'});
            }
            $node->{'structure'}->{'node_'.$direction}
               = $node->{'structure'}->{'menu_'.$direction};
          }
        }
      } elsif (not $node->{'structure'}
               or not $node->{'structure'}->{'node_next'}) {
        # use first menu entry if available as next for Top
        if ($node->{'structure'} and $node->{'structure'}->{'menu_child'}) {
          $node->{'structure'}->{'node_next'} = $node->{'structure'}->{'menu_child'};
          if (!$node->{'structure'}->{'menu_child'}->{'extra'}->{'manual_content'}
              and !$node->{'structure'}->{'menu_child'}->{'structure'}->{'node_prev'}) {
            $node->{'structure'}->{'menu_child'}->{'structure'}->{'node_prev'} = $node;
          }
        } else {
          # use the first non top node as next for Top
          foreach my $first_non_top_node (@{$nodes_list}) {
            if ($first_non_top_node ne $node) {
              #$node->{'structure'} = {} if (! $node->{'structure'});
              $node->{'structure'}->{'node_next'} = $first_non_top_node;
              if (scalar(@{$first_non_top_node->{'extra'}->{'nodes_manuals'}}) == 1) {
                #$first_non_top_node->{'structure'} = {} if (! $first_non_top_node->{'structure'});
                $first_non_top_node->{'structure'}->{'node_prev'} = $node;
              }
              last;
            }
          }
        }
      }
    }
    # check consistency between node pointer and node entries menu order
    if ($node->{'extra'}->{'normalized'} ne 'Top') {
      foreach my $direction (@node_directions) {
        if ($customization_information->get_conf('CHECK_NORMAL_MENU_STRUCTURE')
            and $node->{'structure'}->{'node_'.$direction}
            and $node->{'structure'}->{'menu_'.$direction}
            and $node->{'structure'}->{'menu_'.$direction}
               ne $node->{'structure'}->{'node_'.$direction}
            and not $node->{'structure'}->{'menu_'.$direction}->{'extra'}->{'manual_content'}) {
          $registrar->line_warn($customization_information,
            sprintf(__("node %s pointer for `%s' is `%s' but %s is `%s' in menu"),
                  $direction,
                  node_extra_to_texi($node->{'extra'}),
                  node_extra_to_texi($node->{'structure'}->{'node_'.$direction}->{'extra'}),
                  $direction,
                  node_extra_to_texi($node->{'structure'}->{'menu_'.$direction}->{'extra'})),
                 $node->{'source_info'});
        }
      }
    }

    # check for node up / menu up mismatch
    if ($customization_information->get_conf('CHECK_NORMAL_MENU_STRUCTURE')
        and $node->{'structure'}->{'node_up'}
        # No check if node up is an external manual
        and (!$node->{'structure'}->{'node_up'}->{'extra'}->{'manual_content'})
        and (!$node->{'structure'}->{'menu_up_hash'}
          or !$node->{'structure'}->{'menu_up_hash'}->{$node->{'structure'}->{'node_up'}->{'extra'}->{'normalized'}})) {
      # check if up node has a menu
      if ($node->{'structure'}->{'node_up'}->{'extra'}->{'menus'}
          and @{$node->{'structure'}->{'node_up'}->{'extra'}->{'menus'}}) {
        $registrar->line_warn($customization_information,
         sprintf(
           __("node `%s' lacks menu item for `%s' despite being its Up target"),
           node_extra_to_texi($node->{'structure'}->{'node_up'}->{'extra'}),
           node_extra_to_texi($node->{'extra'})),
         $node->{'structure'}->{'node_up'}->{'source_info'});
      }
      # FIXME check that the menu_up_hash is not empty (except for Top)?
      # FIXME check that node_up is not an external node (except for Top)?
    }
  }
}


# set node directions based on sectioning and @node explicit directions
sub nodes_tree($$$$$)
{
  my $registrar = shift;
  my $customization_information = shift;
  my $parser_information = shift;
  my $nodes_list = shift;
  my $labels = shift;
  return undef unless ($nodes_list and @{$nodes_list});

  my $top_node;
  # Go through all the nodes and set directions.
  foreach my $node (@{$nodes_list}) {
    if ($node->{'extra'}->{'normalized'} eq 'Top') {
      $top_node = $node;
    }
    my $automatic_directions
        = (scalar(@{$node->{'extra'}->{'nodes_manuals'}}) == 1);

    if ($automatic_directions) {
      if ($node->{'extra'}->{'normalized'} ne 'Top') {
        foreach my $direction (@node_directions) {
          # prev already defined for the node first Top node menu entry
          if ($direction eq 'prev' and $node->{'node_'.$direction}
              and $node->{'structure'}->{'node_'.$direction}->{'extra'}
              and $node->{'structure'}->{'node_'.$direction}->{'extra'}->{'normalized'}
              and $node->{'structure'}->{'node_'.$direction}->{'extra'}->{'normalized'} eq 'Top') {
            next;
          }
          if ($node->{'extra'}->{'associated_section'}) {
            my $section = $node->{'extra'}->{'associated_section'};

            # Prefer the section associated with a @part for node directions.
            if ($section->{'extra'}->{'part_associated_section'}) {
              $section = $section->{'extra'}->{'part_associated_section'};
            }

            my $direction_associated_node
              = _section_direction_associated_node($section, $direction);
            if ($direction_associated_node) {
              #$node->{'structure'} = {} if (! $node->{'structure'});
              $node->{'structure'}->{'node_'.$direction} = $direction_associated_node;
            }
          }
        }
      } else {
        # Special case for Top node, use first section
        if ($node->{'extra'}->{'associated_section'}
            and $node->{'extra'}->{'associated_section'}->{'structure'}->{'section_childs'}
            and $node->{'extra'}->{'associated_section'}->{'structure'}->{'section_childs'}->[0]
            and $node->{'extra'}->{'associated_section'}->{'structure'}->{'section_childs'}->[0]->{'extra'}->{'associated_node'}) {
          my $top_node_section_child
            = $node->{'extra'}->{'associated_section'}->{'structure'}->{'section_childs'}->[0]->{'extra'}->{'associated_node'};
          #$node->{'structure'} = {} if (! $node->{'structure'});
          $node->{'structure'}->{'node_next'} = $top_node_section_child;
          if (scalar(@{$top_node_section_child->{'extra'}->{'nodes_manuals'}}) == 1) {
            #$top_node_section_child->{'structure'} = {} if (! $top_node_section_child->{'structure'});
            $top_node_section_child->{'structure'}->{'node_prev'} = $node;
          }
        }
      }
    } else { # explicit directions
      my @directions = @{$node->{'extra'}->{'nodes_manuals'}};
      shift @directions;
      foreach my $direction (@node_directions) {
        my $node_direction = shift @directions;
        next if (!defined($node_direction));

        #$node->{'structure'} = {} if (! $node->{'structure'});
        # external node
        if ($node_direction->{'manual_content'}) {
          $node->{'structure'}->{'node_'.$direction} = { 'extra' => $node_direction };
        } else {
          if ($labels->{$node_direction->{'normalized'}}) {
            my $node_target
               = $labels->{$node_direction->{'normalized'}};
            $node->{'structure'}->{'node_'.$direction} = $node_target;

            if (!$customization_information->get_conf('novalidate')
                and !_check_node_same_texinfo_code($node_target,
                                                   $node_direction)) {
              $registrar->line_warn($customization_information,
                sprintf(
                  __("%s pointer `%s' (for node `%s') different from %s name `%s'"),
                  $direction_texts{$direction},
                  node_extra_to_texi($node_direction),
                  node_extra_to_texi($node->{'extra'}),
                                     $node_target->{'cmdname'},
                  node_extra_to_texi($node_target->{'extra'})),
                $node->{'source_info'});
            }
          } else {
            if ($customization_information->get_conf('novalidate')) {
              $node->{'structure'}->{'node_'.$direction} = { 'extra' => $node_direction };
            } else {
              $registrar->line_error($customization_information,
                   sprintf(__("%s reference to nonexistent `%s'"),
                      $direction_texts{$direction},
                      node_extra_to_texi($node_direction)),
                   $node->{'source_info'});
            }
          }
        }
      }
    }
  }
  $top_node = $nodes_list->[0] if (!$top_node);

  return $top_node;
}

sub section_level_adjusted_command_name($)
{
  my $element = shift;

  my $heading_level = $element->{'structure'}->{'section_level'};
  my $command;
  if ($heading_level ne $Texinfo::Common::command_structuring_level{$element->{'cmdname'}}) {
    $command
      = $Texinfo::Common::level_to_structuring_command{$element->{'cmdname'}}->[$heading_level];
  } else {
    $command = $element->{'cmdname'};
  }
  return $command;
}

# Return a list of tree units to be converted into pages.  Each tree unit
# starts with a @node as its first child (except possibly the first one).
sub split_by_node($)
{
  my $root = shift;
  my $tree_units;
  my $current = { 'type' => 'unit' };
  push @$tree_units, $current;
  my @pending_parts = ();
  foreach my $content (@{$root->{'contents'}}) {
    if ($content->{'cmdname'} and $content->{'cmdname'} eq 'part') {
      push @pending_parts, $content;
      next;
    }
    if ($content->{'cmdname'} and $content->{'cmdname'} eq 'node') {
      if (not $current->{'extra'} or not $current->{'extra'}->{'unit_command'}) {
        #$current->{'extra'} = {} if (! $current->{'extra'});
        $current->{'extra'}->{'unit_command'} = $content;
      } else {
        $current = { 'type' => 'unit', 'extra' => {'unit_command' => $content},
                    'structure' => {'unit_prev' => $tree_units->[-1]}};
        #$tree_units->[-1]->{'structure'} = {} if (! $tree_units->[-1]->{'structure'});
        $tree_units->[-1]->{'structure'}->{'unit_next'} = $current;
        push @$tree_units, $current;
      }
    }
    if (@pending_parts) {
      foreach my $part (@pending_parts) {
        push @{$current->{'contents'}}, $part;
        $part->{'structure'}->{'associated_unit'} = $current;
      }
      @pending_parts = ();
    }
    push @{$current->{'contents'}}, $content;
    #$content->{'structure'} = {} if (! $content->{'structure'});
    $content->{'structure'}->{'associated_unit'} = $current;
  }
  if (@pending_parts) {
    foreach my $part (@pending_parts) {
      push @{$current->{'contents'}}, $part;
      $part->{'structure'}->{'associated_unit'} = $current;
    }
    @pending_parts = ();
  }

  return $tree_units;
}

# Return a list of tree units to be converted into pages.  Each tree unit
# starts with the @node associated with a sectioning command or with the
# sectioning command if there is no associated node
sub split_by_section($)
{
  my $root = shift;
  my $tree_units;
  my $current = { 'type' => 'unit' };
  push @$tree_units, $current;
  foreach my $content (@{$root->{'contents'}}) {
    if ($content->{'cmdname'}
        and (($content->{'cmdname'} eq 'node'
              and $content->{'extra'}->{'associated_section'})
             or ($content->{'cmdname'} eq 'part'
                 and $content->{'extra'}->{'part_associated_section'}))) {
      my $new_section;
      if ($content->{'cmdname'} eq 'node') {
        $new_section = $content->{'extra'}->{'associated_section'};
      } else {
        $new_section = $content->{'extra'}->{'part_associated_section'};
      }
      if (not defined($current->{'extra'})
               or not defined($current->{'extra'}->{'unit_command'})) {
        #$current->{'extra'} = {} if (!$current->{'extra'});
        $current->{'extra'}->{'unit_command'} = $new_section;
      } elsif (!$current->{'extra'}->{'unit_command'}
              or $new_section ne $current->{'extra'}->{'unit_command'}) {
        $current = { 'type' => 'unit',
                     'extra' => {'unit_command' => $new_section},
                     'structure' => {'unit_prev' => $tree_units->[-1]}};
        #$tree_units->[-1]->{'structure'} = {} if (! $tree_units->[-1]->{'structure'});
        $tree_units->[-1]->{'structure'}->{'unit_next'} = $current;
        push @$tree_units, $current;
      }
    } elsif ($content->{'cmdname'} and $content->{'cmdname'} ne 'node'
             and $Texinfo::Commands::root_commands{$content->{'cmdname'}}) {
      if (not defined($current->{'extra'})
               or not defined($current->{'extra'}->{'unit_command'})) {
        #$current->{'extra'} = {} if (! $current->{'extra'});
        $current->{'extra'}->{'unit_command'} = $content;
      } elsif ($current->{'extra'}->{'unit_command'} ne $content) {
        $current = {'type' => 'unit', 'extra' => {'unit_command' => $content},
                    'structure' => {'unit_prev' => $tree_units->[-1]}};
        #$tree_units->[-1]->{'structure'} = {} if (! $tree_units->[-1]->{'structure'});
        $tree_units->[-1]->{'structure'}->{'unit_next'} = $current;
        push @$tree_units, $current;
      }
    }
    push @{$current->{'contents'}}, $content;
    #$content->{'structure'} = {} if (! $content->{'structure'});
    $content->{'structure'}->{'associated_unit'} = $current;
  }
  return $tree_units;
}

# Associate top-level units with pages according to the splitting
# specification.  Set 'first_in_page' on each unit to the unit
# that is the first in the output page.
sub split_pages ($$)
{
  my $tree_units = shift;
  my $split = shift;

  return undef if (!$tree_units or !@$tree_units);

  my $split_level;
  if (!$split) {
    foreach my $tree_unit (@$tree_units) {
      #$tree_unit->{'extra'} = {} if (! $tree_unit->{'extra'});
      $tree_unit->{'extra'}->{'first_in_page'} = $tree_units->[0];
    }
    return;
  } elsif ($split eq 'chapter') {
    $split_level = 1;
  } elsif ($split eq 'section') {
    $split_level = 2;
  } elsif ($split ne 'node') {
    warn "Unknown split specification: $split\n";
  }

  my $current_first_in_page;
  foreach my $tree_unit (@$tree_units) {
    my $level;
    my $section = _tree_unit_section($tree_unit);
    if (defined($section)) {
      $level = $section->{'structure'}->{'section_level'};
    }
    #print STDERR "level($split_level) $level ".root_or_external_element_cmd_texi($tree_unit)."\n";
    if (!defined($split_level) or (defined($level) and $split_level >= $level)
        or !$current_first_in_page) {
      $current_first_in_page = $tree_unit;
    }
    #$tree_unit->{'extra'} = {} if (! $tree_unit->{'extra'});
    $tree_unit->{'extra'}->{'first_in_page'} = $current_first_in_page;
  }
}

# Returns an element associated to a label that can be used to setup a target
# to the label.  If the target is an external node, create such element here,
# if it is a node return the parent element that is supposed to be the
# target for links to the node.  Otherwise there is no such element (yet),
# for floats and anchor, return undef.
sub _label_target_element($)
{
  my $label = shift;
  if ($label->{'extra'} and $label->{'extra'}->{'manual_content'}) {
    my $external_node = { 'type' => 'external_node',
      'extra' => {'manual_content' => $label->{'extra'}->{'manual_content'}}};
  
    if ($label->{'extra'}->{'node_content'}) {
      $external_node->{'extra'}->{'node_content'}
        = $label->{'extra'}->{'node_content'};
      $external_node->{'extra'}->{'normalized'}
        = Texinfo::Convert::NodeNameNormalization::normalize_node(
                   {'contents' => $label->{'extra'}->{'node_content'}});
    }
    return $external_node;
  } elsif ($label->{'cmdname'} and $label->{'cmdname'} eq 'node') {
    return $label->{'structure'}->{'associated_unit'};
  } else {
    # case of a @float or an @anchor, no target element defined at this stage
    return undef;
  }
}

sub _tree_unit_section($)
{
  my $tree_unit = shift;
  if (not defined($tree_unit->{'extra'})
      or not defined($tree_unit->{'extra'}->{'unit_command'})) {
    return undef;
  }
  if ($tree_unit->{'extra'}->{'unit_command'}->{'cmdname'} eq 'node') {
    if ($tree_unit->{'extra'}->{'unit_command'}->{'extra'}->{'associated_section'}) {
      return $tree_unit->{'extra'}->{'unit_command'}->{'extra'}->{'associated_section'};
    } else {
      return undef;
    }
  } else {
    return $tree_unit->{'extra'}->{'unit_command'};
  }
}

sub _tree_unit_node($)
{
  my $tree_unit = shift;
  if (not defined($tree_unit->{'extra'})
      or not defined($tree_unit->{'extra'}->{'unit_command'})) {
    return undef;
  }
  if ($tree_unit->{'extra'}->{'unit_command'}->{'cmdname'} eq 'node') {
    return $tree_unit->{'extra'}->{'unit_command'};
  } else {
    if ($tree_unit->{'extra'}->{'unit_command'}->{'extra'}->{'associated_node'}) {
      return $tree_unit->{'extra'}->{'unit_command'}->{'extra'}->{'associated_node'}
    } else {
      return undef;
    }
  }
}

# Do element directions (like in texi2html) and store them
# in 'structure->'directions'.
# The directions are only created if pointing to other 'unit' elements.
# In practice there are only tree unit passed to the function, but
# other root elements could probably be used, in theory.
sub elements_directions($$$)
{
  my $customization_information = shift;
  my $labels = shift;
  my $tree_units = shift;
  return if (!$tree_units or !@$tree_units);

  my $node_top = $labels->{'Top'};
  foreach my $tree_unit (@$tree_units) {
    my $directions = {};
    $directions->{'This'} = $tree_unit;
    $directions->{'Forward'} = $tree_unit->{'structure'}->{'unit_next'}
      if ($tree_unit->{'structure'}
          and $tree_unit->{'structure'}->{'unit_next'}
          and defined($tree_unit->{'structure'}->{'unit_next'}->{'type'})
          and $tree_unit->{'structure'}->{'unit_next'}->{'type'} eq 'unit');
    $directions->{'Back'} = $tree_unit->{'structure'}->{'unit_prev'}
      if ($tree_unit->{'structure'}
          and $tree_unit->{'structure'}->{'unit_prev'}
          and defined($tree_unit->{'structure'}->{'unit_prev'}->{'type'})
          and $tree_unit->{'structure'}->{'unit_prev'}->{'type'} eq 'unit');
    my $node = _tree_unit_node($tree_unit);
    if (defined($node)) {
      foreach my $direction(['NodeUp', 'node_up'], ['NodeNext', 'node_next'],
                            ['NodePrev', 'node_prev']) {
        $directions->{$direction->[0]}
           = _label_target_element($node->{'structure'}->{$direction->[1]})
            if ($node->{'structure'}->{$direction->[1]});
      }
      # Now do NodeForward which is something like the following node.
      my $automatic_directions =
        (scalar(@{$node->{'extra'}->{'nodes_manuals'}}) == 1);
      if ($node->{'structure'}->{'menu_child'}) {
        $directions->{'NodeForward'} = _label_target_element($node->{'structure'}->{'menu_child'});
      } elsif ($automatic_directions and $node->{'associated_section'}
               and $node->{'associated_section'}->{'structure'}->{'section_childs'}
               and $node->{'associated_section'}->{'structure'}->{'section_childs'}->[0]) {
        $directions->{'NodeForward'}
          = $node->{'associated_section'}->{'structure'}->{'section_childs'}->[0]->{'structure'}->{'associated_unit'};
      } elsif ($node->{'structure'}->{'node_next'}) {
        $directions->{'NodeForward'} = _label_target_element($node->{'structure'}->{'node_next'});
      } elsif ($node->{'structure'}->{'node_up'}) {
        my $up = $node->{'structure'}->{'node_up'};
        my @up_list = ($node);
        # the condition with the up_list avoids infinite loops
        # the last condition stops when the Top node is reached.
        while (not (grep {$up eq $_} @up_list
                    or ($node_top and $up eq $node_top))) {
          if ($up->{'structure'} and defined($up->{'structure'}->{'node_next'})) {
            $directions->{'NodeForward'} = _label_target_element($up->{'structure'}->{'node_next'});
            last;
          }
          push @up_list, $up;
          last if (not $up->{'structure'} or not $up->{'structure'}->{'node_up'});
          $up = $up->{'structure'}->{'node_up'};
        }
      }
      
      if ($directions->{'NodeForward'}
          and $directions->{'NodeForward'}->{'type'} eq 'unit'
          and (!$directions->{'NodeForward'}->{'structure'}
               or !$directions->{'NodeForward'}->{'structure'}->{'directions'}
               or !$directions->{'NodeForward'}->{'structure'}->{'directions'}->{'NodeBack'})) {
        #$directions->{'NodeForward'}->{'structure'} = {} if (! $directions->{'NodeForward'}->{'structure'});
        #$directions->{'NodeForward'}->{'structure'}->{'directions'} = {} if (! $directions->{'NodeForward'}->{'structure'}->{'directions'});
        $directions->{'NodeForward'}->{'structure'}->{'directions'}->{'NodeBack'} = $tree_unit;
      }
    }
    my $section = _tree_unit_section($tree_unit);
    if (not defined($section)) {
      # If there is no associated section, find the previous element section.
      # Use the FastForward of this element.
      # Use it as FastBack if the section is top level, or use the FastBack.
      my $section_element;
      my $current = $tree_unit;
      while ($current->{'structure'} and $current->{'structure'}->{'unit_prev'}) {
        $current = $current->{'structure'}->{'unit_prev'};
        $section = _tree_unit_section($current);
        if (defined($section)) {
          $section_element = $current;
          last;
        }
      }
      if ($section_element) {
        if ($section_element->{'structure'}->{'directions'}->{'FastForward'}) {
          $directions->{'FastForward'}
            = $section_element->{'structure'}->{'directions'}->{'FastForward'};
        }
        if ($section->{'structure'}->{'section_level'} <= 1) {
          $directions->{'FastBack'} = $section_element;
        } elsif ($section_element->{'structure'}->{'directions'}->{'Fastback'}) {
          $directions->{'FastBack'}
            = $section_element->{'structure'}->{'directions'}->{'Fastback'};
        }
      }
    } else {
      foreach my $direction(['Up', 'section_up'], ['Next', 'section_next'],
                            ['Prev', 'section_prev']) {
        # in most cases $section->{$direction->[1]}->{'structure'}->{'associated_unit'} is defined
        # but it may not be the case for the up of @top.
        # The section may be its own up in cases like
        #  @part part
        #  @chapter chapter
        # in that cas the direction is not set up
        $directions->{$direction->[0]} = $section->{'structure'}->{$direction->[1]}->{'structure'}->{'associated_unit'}
          if ($section->{'structure'}->{$direction->[1]}
              and $section->{'structure'}->{$direction->[1]}->{'structure'}->{'associated_unit'}
              and $section->{'structure'}->{$direction->[1]}->{'structure'}->{'associated_unit'} ne $section->{'structure'}->{'associated_unit'});
      }

      my $up = $section;
      while ($up->{'structure'}->{'section_level'} > 1
             and $up->{'structure'}->{'section_up'}) {
        $up = $up->{'structure'}->{'section_up'};
      }

      # fastforward is the next element on same level than the upper parent
      # element.
      if ($up->{'structure'}->{'section_level'} < 1
          and $up->{'cmdname'} and $up->{'cmdname'} eq 'top'
          and $up->{'structure'}->{'section_childs'}
          and @{$up->{'structure'}->{'section_childs'}}) {
        $directions->{'FastForward'}
           = $up->{'structure'}->{'section_childs'}->[0]->{'structure'}->{'associated_unit'};
      } elsif ($up->{'structure'}->{'toplevel_next'}) {
        $directions->{'FastForward'} = $up->{'structure'}->{'toplevel_next'}->{'structure'}->{'associated_unit'};
      } elsif ($up->{'structure'}->{'section_next'}) {
        $directions->{'FastForward'} = $up->{'structure'}->{'section_next'}->{'structure'}->{'associated_unit'};
      }
      # if the element isn't at the highest level, fastback is the
      # highest parent element
      if ($up and $up ne $section) {
        $directions->{'FastBack'} = $up->{'structure'}->{'associated_unit'};
      } elsif ($section->{'structure'}->{'section_level'} <= 1
               and $directions->{'FastForward'}) {
        # the element is a top level element, we adjust the next
        # toplevel element fastback
        #$directions->{'FastForward'}->{'structure'} = {} if (! $directions->{'FastForward'}->{'structure'});
        #$directions->{'FastForward'}->{'structure'}->{'directions'} = {} if (! $directions->{'FastForward'}->{'structure'}->{'directions'});
        $directions->{'FastForward'}->{'structure'}->{'directions'}->{'FastBack'}
          = $tree_unit if ($directions and $directions->{'FastForward'});
      }
    }
    # Use node up for Up if there is no section up.
    # Not done in the default case.
    if ($customization_information->get_conf('USE_UP_NODE_FOR_ELEMENT_UP')
        and !$directions->{'Up'} and defined($node)
        and $node->{'structure'}->{'node_up'}
        and (!$node_top or ($node ne $node_top))) {
      #print STDERR "Node for up: ".root_or_external_element_cmd_texi($tree_unit)."\n";
      my $up_node_element
        = _label_target_element($node->{'structure'}->{'node_up'});
      $directions->{'Up'} = $up_node_element if ($up_node_element);
    }
    #$tree_unit->{'structure'} = {} if (! $tree_unit->{'structure'});
    if ($tree_unit->{'structure'}->{'directions'}) {
      %{$tree_unit->{'structure'}->{'directions'}} = (%{$tree_unit->{'structure'}->{'directions'}},
                                                %$directions)
    } else {
      $tree_unit->{'structure'}->{'directions'} = $directions;
    }
  }
  if ($customization_information->get_conf('DEBUG')) {
    foreach my $tree_unit (@$tree_units) {
      print STDERR "Directions($tree_unit): "
         . print_element_directions($tree_unit)."\n";
    }
  }
}

# for now, the elements can only be tree units.  It could probably
# be other root elements for which file directions make sense, in theory.
sub elements_file_directions($)
{
  my $tree_units = shift;
  return if (!$tree_units or !@$tree_units);

  my $current_filename;
  my $first_element_in_file;
  # need to gather the directions before the FirstInFile* directions
  # are added to the first element in the file.
  my @first_element_in_file_directions;
  foreach my $tree_unit (@$tree_units) {
    my $directions;
    my $filename;
    if (defined($tree_unit->{'structure'}->{'unit_filename'})) {
      $filename = $tree_unit->{'structure'}->{'unit_filename'};
      my $current_tree_unit = $tree_unit;
      if (not defined($current_filename)
          or $filename ne $current_filename) {
        $first_element_in_file = $tree_unit;
        @first_element_in_file_directions = keys %{$tree_unit->{'structure'}->{'directions'}};
        $current_filename = $filename;
      }
      while ($current_tree_unit->{'structure'}->{'unit_prev'}) {
        $current_tree_unit = $current_tree_unit->{'structure'}->{'unit_prev'};
        if (defined($current_tree_unit->{'structure'}->{'unit_filename'})) {
          if ($current_tree_unit->{'structure'}->{'unit_filename'} ne $filename) {
            $tree_unit->{'structure'}->{'directions'}->{'PrevFile'}
                 = $current_tree_unit;
            last;
          }
        } else {
          last;
        }
      }
      $current_tree_unit = $tree_unit;
      while ($current_tree_unit->{'structure'}->{'unit_next'}) {
        $current_tree_unit = $current_tree_unit->{'structure'}->{'unit_next'};
        if (defined($current_tree_unit->{'structure'}->{'unit_filename'})) {
          if ($current_tree_unit->{'structure'}->{'unit_filename'} ne $filename) {
            $tree_unit->{'structure'}->{'directions'}->{'NextFile'}
               = $current_tree_unit;
            last;
          }
        } else {
          last;
        }
      }
    }
    # set the directions of the first elements in file, to
    # be used in footers for example
    if (defined($first_element_in_file)) {
      foreach my $first_in_file_direction
                (@first_element_in_file_directions) {
        $tree_unit->{'structure'}->{'directions'}->{'FirstInFile'.$first_in_file_direction}
          = $first_element_in_file->{'structure'}->{'directions'}->{$first_in_file_direction};
      }
    }
  }
}

# used in debug messages
sub root_or_external_element_cmd_texi($)
{
  my $element = shift;
  if (!$element) {
    return "UNDEF ELEMENT";
  }
  if (!$element->{'type'}) {
    return "element $element without type: ".
       Texinfo::Common::debug_print_element_details($element);
  }

  if ($element->{'type'} eq 'external_node') {
    my $command = {'contents' => [{'text' => '('},
                        @{$element->{'extra'}->{'manual_content'}},
                               {'text' => ')'}]};
    if ($element->{'extra'}->{'node_content'}) {
      unshift @{$command->{'contents'}}, @{$element->{'extra'}->{'node_content'}};
    }
    return Texinfo::Convert::Texinfo::convert_to_texinfo($command);
  }
  
  my $command_element;
  if ($element->{'extra'} and $element->{'extra'}->{'unit_command'}) {
    $command_element = $element->{'extra'}->{'unit_command'};
  } else {
    # happens when there are only nodes and sections are used as elements
    my $result = "No associated command ";
    $result .= "(type $element->{'type'})" if (defined($element->{'type'}));
    return $result;
  }
  return Texinfo::Convert::Texinfo::root_heading_command_to_texinfo($command_element);
}

# Used for debugging and in test suite, but not generally useful. Not
# documented in pod section and not exportable as it should not, in
# general, be used.
# In general would be called with tree unit elements, but is more generic
# to account for other situations.
sub print_element_directions($)
{
  my $element = shift;
  my $result = 'element: '.root_or_external_element_cmd_texi($element)."\n";

  if ($element->{'extra'} and $element->{'structure'}->{'directions'}) {
    foreach my $direction (sort(keys(%{$element->{'structure'}->{'directions'}}))) {
      $result .= "  $direction: ".
       root_or_external_element_cmd_texi(
         $element->{'structure'}->{'directions'}->{$direction})."\n";
    }
  } else {
    $result .= "  NO DIRECTION";
  }
  return $result;
}

# For each internal reference command, set the 'normalized' key in the
# 'menu_entry_node' for menu entries, and 'node_argument' for @*ref.
# Set the 'label' key in the 'extra' hash of the reference tree element
# with 'node_argument' to the associated labeled tree element.
sub associate_internal_references($$$$$)
{
  my $registrar = shift;
  my $customization_information = shift;
  my $parser_information = shift;
  my $labels = shift;
  my $refs = shift;

  return if (!defined($refs));
  foreach my $ref (@$refs) {
    my $node_arg;
    $node_arg = $ref->{'extra'}->{'menu_entry_node'};
    
    if (defined $node_arg) {
      if ($node_arg->{'node_content'}) {
        my $normalized =
             Texinfo::Convert::NodeNameNormalization::normalize_node(
                {'contents' => $node_arg->{'node_content'} });
        $node_arg->{'normalized'} = $normalized
          if (defined $normalized and $normalized ne '');
      }
      next;
    }
    
    $node_arg = $ref->{'extra'}->{'node_argument'};
    if ($node_arg and $node_arg->{'node_content'}) {
      my $normalized =
           Texinfo::Convert::NodeNameNormalization::normalize_node(
              {'contents' => $node_arg->{'node_content'} });
      $node_arg->{'normalized'} = $normalized;

      if (!defined($labels->{$node_arg->{'normalized'}})) {
        if (!$customization_information->get_conf('novalidate')) {
          $registrar->line_error($customization_information,
              sprintf(__("\@%s reference to nonexistent node `%s'"),
                  $ref->{'cmdname'}, node_extra_to_texi($node_arg)),
                  $ref->{'source_info'});
        }
      } else {
        my $node_target = $labels->{$node_arg->{'normalized'}};
        $ref->{'extra'}->{'label'} = $node_target;
        if (!$customization_information->get_conf('novalidate')
            and !_check_node_same_texinfo_code($node_target, $node_arg)) {
          $registrar->line_warn($customization_information,
             sprintf(__("\@%s to `%s', different from %s name `%s'"),
                 $ref->{'cmdname'},
                 node_extra_to_texi($node_arg),
                 $node_target->{'cmdname'},
                 node_extra_to_texi($node_target->{'extra'})),
             $ref->{'source_info'});
        }
      }
    }
  }
}

sub number_floats($)
{
  my $floats = shift;
  return if (!defined($floats));
  foreach my $style (keys(%$floats)) {
    my %nr_in_chapter;
    my $float_index = 0;
    foreach my $float (@{$floats->{$style}}) {
      next if (!$float->{'extra'}
               or !defined($float->{'extra'}->{'node_content'}));
      $float_index++;
      my $number;
      if ($float->{'extra'}->{'float_section'}) {
        my $up = $float->{'extra'}->{'float_section'};
        while ($up->{'structure'}->{'section_up'}
               and defined($up->{'structure'}->{'section_up'}->{'cmdname'})
               and $command_structuring_level{$up->{'structure'}->{'section_up'}->{'cmdname'}}) {
          $up = $up->{'structure'}->{'section_up'};
        }
        if (!$unnumbered_commands{$up->{'cmdname'}}) {
          $nr_in_chapter{$up->{'structure'}->{'section_number'}}++;
          $number = $up->{'structure'}->{'section_number'} .
            '.' . $nr_in_chapter{$up->{'structure'}->{'section_number'}};
        }
      }
      $number = $float_index if (!defined($number));
      #$float->{'structure'} = {} if (! $float->{'structure'});
      $float->{'structure'}->{'float_number'} = $number;
    }
  }
}

sub get_node_node_childs_from_sectioning
{
  my ($node) = @_;

  my @node_childs;

  if ($node->{'extra'}->{'associated_section'}->{'structure'}->{'section_childs'}) {
    foreach my $child (@{$node->{'extra'}->{'associated_section'}->{'structure'}->{'section_childs'}}) {
      if ($child->{'extra'} and $child->{'extra'}->{'associated_node'}) {
        push @node_childs, $child->{'extra'}->{'associated_node'};
      }
    }
  }
  # Special case for @top.  Gather all the children of the @part following
  # @top.
  if ($node->{'extra'}->{'associated_section'}->{'cmdname'} eq 'top') {
    my $current = $node->{'extra'}->{'associated_section'};
    while ($current->{'structure'}->{'section_next'}) {
      $current = $current->{'structure'}->{'section_next'};
      if ($current->{'cmdname'} and $current->{'cmdname'} eq 'part'
          and $current->{'structure'}->{'section_childs'}) {
        foreach my $child (@{$current->{'structure'}->{'section_childs'}}) {
          if ($child->{'extra'} and $child->{'extra'}->{'associated_node'}) {
            push @node_childs, $child->{'extra'}->{'associated_node'};
          }
        }
      } elsif ($current->{'extra'}->{'associated_node'}) {
        # for @appendix, and what follows, as it stops a @part, but is
        # not below @top
        push @node_childs, $current->{'extra'}->{'associated_node'};
      }
    }
  }
  return @node_childs;
}

# returns the texinfo tree corresponding to a single menu entry pointing to $NODE.
# if $USE_SECTIONS is set, use the section name as menu entry name.
sub new_node_menu_entry
{
  my ($node, $use_sections) = @_;

  my $node_contents;
  $node_contents = $node->{'extra'}->{'node_content'} if ($node->{'extra'});

  # can happen with node without argument or with empty argument
  return undef if (not defined($node_contents));
  
  my ($name_contents, $menu_entry_name);
  if ($use_sections) {
    if (defined $node->{'extra'}->{'associated_section'}) {
      $name_contents
        = $node->{'extra'}->{'associated_section'}->{'args'}->[0]->{'contents'};
    } else {
      $name_contents = $node_contents; # shouldn't happen
    }
  }

  if ($use_sections) {
    $menu_entry_name = {'type' => 'menu_entry_name'};
    $menu_entry_name->{'contents'}
        = Texinfo::Common::copy_contents($name_contents);
    foreach my $content (@{$menu_entry_name->{'contents'}}) {
      $content->{'parent'} = $menu_entry_name;
    }
    # colons could be doubly protected, but it is probably better
    # than not protected at all.
    Texinfo::Common::protect_colon_in_tree($menu_entry_name);
  }

  my $entry = {'type' => 'menu_entry',
               'extra' => {'menu_entry_name' => $menu_entry_name}};

  my $menu_entry_node = {'type' => 'menu_entry_node'};
  $menu_entry_node->{'contents'}
    = Texinfo::Common::copy_contents($node_contents);

  foreach my $content (@{$menu_entry_node->{'contents'}}) {
    $content->{'parent'} = $menu_entry_node;
  }
  # do not protect here, as it could already be protected, and
  # the menu entry should be the same as the node
  #Texinfo::Common::protect_colon_in_tree($menu_entry_node);

  my $description = {'type' => 'menu_entry_description',
                     'contents' => []};
  $description->{'contents'}->[0] = {'type' => 'preformatted',
                                     'parent' => $description,
                                     'contents' => []};
  $description->{'contents'}->[0]->{'contents'}->[0] = {'text' => "\n",
                           'parent' => $description->{'contents'}->[0]};

  if ($use_sections) {
    $entry->{'args'}
     = [{'text' => '* ', 'type' => 'menu_entry_leading_text'},
        $menu_entry_name,
        {'text' => ': ', 'type' => 'menu_entry_separator'},
        $menu_entry_node,
        {'text' => '.', 'type' => 'menu_entry_separator'},
        $description];
  } else {
    $entry->{'args'}
     = [{'text' => '* ', 'type' => 'menu_entry_leading_text'},
        $menu_entry_node,
        {'text' => '::', 'type' => 'menu_entry_separator'},
        $description];
  }

  foreach my $arg(@{$entry->{'args'}}) {
    $arg->{'parent'} = $entry;
  }

  my $modified_node_content;
  ($entry->{'extra'}->{'menu_entry_node'}, $modified_node_content) =
    Texinfo::Common::parse_node_manual($menu_entry_node);
  $menu_entry_node->{'contents'} = $modified_node_content;
  my $content = $entry->{'extra'}->{'menu_entry_node'}->{'node_content'};
  if ($content) {
    $entry->{'extra'}->{'menu_entry_node'}->{'normalized'}
     = Texinfo::Convert::NodeNameNormalization::normalize_node(
                                        {'contents' => $content } );
  }

  $entry->{'extra'}->{'menu_entry_description'} = $description;

  return $entry;
}

sub new_block_command($$$)
{
  my $block_contents = shift;
  my $parent = shift;
  my $command_name = shift;

  my $new_block = {'cmdname' => $command_name, 'parent' => $parent};
  $new_block->{'args'} = [{'type' => 'block_line_arg', 'parent' => $new_block,
                           'extra' => { 'spaces_after_argument' => "\n",}}];

  foreach my $content (@$block_contents) {
    confess("new_block_command: undef \$block_contents content")
      if (!defined($content));
    $content->{'parent'} = $new_block;
  }

  my $end = {'cmdname' => 'end', 'parent' => $new_block,
             'extra' => {'spaces_before_argument' => ' ',
                         'text_arg' => $command_name}};
  $end->{'args'} = [{'type' => 'line_arg', 'parent' => $end,
                     'extra' => {'spaces_after_argument' => "\n"}}];
  push @{$end->{'args'}->[0]->{'contents'}},
         {'text' => $command_name, 'parent' => $end->{'args'}->[0]};

  $new_block->{'contents'} = [@$block_contents, $end];

  return $new_block;
}

sub new_complete_node_menu
{
  my ($node, $use_sections) = @_;

  my @node_childs = get_node_node_childs_from_sectioning($node);

  if (not scalar(@node_childs)) {
    return;
  }

  my @pending;
  foreach my $child (@node_childs) {
    my $entry = new_node_menu_entry($child, $use_sections);
    push @pending, $entry if defined($entry);
  }

  my $section = $node->{'extra'}->{'associated_section'};
  my $new_menu = new_block_command(\@pending, $section, 'menu');

  return $new_menu;
}

sub _sort_string($$)
{
  my $a = shift;
  my $b = shift;
  return (($a =~ /^[[:alpha:]]/ and $b =~ /^[[:alpha:]]/)
              or ($a !~ /^[[:alpha:]]/ and $b !~ /^[[:alpha:]]/))
              ? ($a cmp $b)
                : (($a =~ /^[[:alpha:]]/ && 1) || -1);
}

sub _sort_index_entries($$)
{
  my $key1 = shift;
  my $key2 = shift;
  my $a = uc($key1->{'key'});
  my $b = uc($key2->{'key'});
  my $res = _sort_string($a, $b);
  if ($res == 0) {
    $res = ($key1->{'number'} <=> $key2->{'number'});
  }
  # This may happen if 2 indices are merged as the number is per
  # index name.  The @-command should be different though, for
  # index names to be different.
  if ($res == 0) {
    $res = ($key1->{'index_at_command'} cmp $key2->{'index_at_command'});
  }
  return $res;
}

sub setup_index_entry_keys_formatting($)
{
  my $customization_information = shift;

  # TODO it would probably be relevant to set the encoding by setting the
  # copy_options_for_convert_text optional argument when we use
  # Unicode::Collate to sort.  Otherwise it is better not to as the result
  # is worse when setting it.
  my $options = {'sort_string' => 1, 'ascii_punctuation' => 1,
   Texinfo::Convert::Text::copy_options_for_convert_text(
                                  $customization_information)};

  return $options;
}

# can be used for subentries
sub index_entry_sort_string($$$$)
{
  my $main_entry = shift;
  my $entry_tree_element = shift;
  my $sortas = shift;
  my $options = shift;

  my $convert_to_text_options = {%$options};
  $convert_to_text_options->{'code'} = $main_entry->{'in_code'};

  my $entry_key;
  if (defined($sortas)) {
    $entry_key = $sortas;
  } else {
    $entry_key = Texinfo::Convert::Text::convert_to_text(
                          $entry_tree_element, $convert_to_text_options);
    # FIXME do that for sortas too?
    if (exists($main_entry->{'index_ignore_chars'})) {
      my $ignore_chars
         = quotemeta(join('', keys(%{$main_entry->{'index_ignore_chars'}})));
      if ($ignore_chars ne '') {
        $entry_key =~ s/[$ignore_chars]//g;
      }
    }
  }
  # This avoids varying results depending on whether the string is
  # represented internally in UTF-8.  See "the Unicode bug" in the
  # "perlunicode" man page.
  utf8::upgrade($entry_key);

  return $entry_key;
}

# the structure returned depends on $SORT_BY_LETTER being set
# or not.  It is described in the pod documentation.
sub sort_indices($$$;$)
{
  my $registrar = shift;
  my $customization_information = shift;
  my $index_entries = shift;
  my $sort_by_letter = shift;

  my $options = setup_index_entry_keys_formatting($customization_information);
  my $sorted_index_entries;
  my $index_entries_sort_strings = {};
  return $sorted_index_entries, $index_entries_sort_strings
    unless ($index_entries);
  $sorted_index_entries = {};
  foreach my $index_name (keys(%$index_entries)) {
    # used if not $sort_by_letter
    my $sortable_index_entries = [];
    # used if $sort_by_letter
    my $index_letter_hash = {};
    foreach my $entry (@{$index_entries->{$index_name}}) {
      my $entry_key = index_entry_sort_string($entry,
                                 {'contents' => $entry->{'entry_content'}},
                                              $entry->{'sortas'}, $options);
      $index_entries_sort_strings->{$entry} = $entry_key;
      if ($entry_key !~ /\S/) {
        $registrar->line_warn($customization_information,
                     sprintf(__("empty index key in \@%s"),
                                 $entry->{'index_at_command'}),
                        $entry->{'entry_element'}->{'source_info'});
      } else {
        my $sortable_entry = {'entry' => $entry, 'key' => $entry_key,
           'number' => $entry->{'entry_number'},
           'index_at_command' => $entry->{'index_at_command'}};

        if ($sort_by_letter) {
          my $letter = uc(substr($entry_key, 0, 1));
          push @{$index_letter_hash->{$letter}}, $sortable_entry;
        } else {
          push @{$sortable_index_entries}, $sortable_entry;
        }
      }
    }
    if ($sort_by_letter) {
      foreach my $letter (sort _sort_string (keys %$index_letter_hash)) {
        my @sorted_letter_entries
           = map {$_->{'entry'}}
               sort _sort_index_entries @{$index_letter_hash->{$letter}};
        push @{$sorted_index_entries->{$index_name}},
          { 'letter' => $letter, 'entries' => \@sorted_letter_entries };
      }
    } else {
      $sorted_index_entries->{$index_name} = [
        map {$_->{'entry'}}
          sort _sort_index_entries @{$sortable_index_entries}
      ];
    }
  }
  return $sorted_index_entries, $index_entries_sort_strings;
}

sub merge_indices($)
{
  my $index_names = shift;

  my $merged_index_entries;
  foreach my $index_name (keys(%$index_names)) {
    my $index_info = $index_names->{$index_name};
    next if ($index_info->{'merged_in'});
    foreach my $contained_index (keys (%{$index_info->{'contained_indices'}})) {
      if ($index_names->{$contained_index}->{'index_entries'}) {
        $merged_index_entries = {} if (! $merged_index_entries);
        push @{$merged_index_entries->{$index_name}},
          @{$index_names->{$contained_index}->{'index_entries'}};
      }
    }
  }
  return $merged_index_entries;
}


1;

__END__

=head1 NAME

Texinfo::Structuring - information on Texinfo::Parser tree

=head1 SYNOPSIS

  use Texinfo::Structuring qw(sectioning_structure nodes_tree number_floats
    associate_internal_references split_by_node split_by_section split_pages
    merge_indices sort_indices elements_directions elements_file_directions);

  # $tree is a Texinfo document tree.  $parser is a Texinfo::Parser object.
  # $config is an object implementing the get_conf() method.
  my $registrar = $parser->registered_errors();
  my $sections_root = sectioning_structure ($registrar, $config, $tree);
  my ($labels, $targets_list, $nodes_list) = $parser->labels_information();
  my $parser_information = $parser->global_information();
  my $global_commands = $parser->global_commands_information();
  set_menus_node_directions($registrar, $config, $parser_information,
                            $global_commands, $nodes_list, $labels);
  my $top_node = nodes_tree($registrar, $config, $parser_information, $nodes_list, $labels);
  complete_node_tree_with_menus($registrar, $config, $nodes_list, $top_node);
  my $refs = $parser->internal_references_information();
  check_nodes_are_referenced($registrar, $config, $nodes_list, $top_node, $labels, $refs);
  associate_internal_references($registrar, $parser, $parser_information, $labels, $refs);
  number_floats($parser->floats_information());
  my $tree_units;
  if ($split_at_nodes) {
    $tree_units = split_by_node($tree);
  } else {
    $tree_units = split_by_section($tree);
  }
  split_pages($tree_units, $split);
  elements_directions($config, $labels, $tree_units);
  elements_file_directions($tree_units);

  my $index_names = $parser->indices_information();
  my $merged_index_entries
     = merge_indices($index_names);
  my $index_entries_sorted;
  if ($sort_by_letter) {
    $index_entries_sorted = sort_indices($registrar, $config,
                                       $merged_index_entries, 'by_letter');
  } else {
    $index_entries_sorted = sort_indices($registrar, $config,
                                         $merged_index_entries);
  }


=head1 NOTES

The Texinfo Perl module main purpose is to be used in C<texi2any> to convert
Texinfo to other formats.  There is no promise of API stability.

=head1 DESCRIPTION

Texinfo::Structuring first allows to collect information on a Texinfo tree.
In most case, it also requires information from a parser object to do that
job.  Thanks to C<sectioning_structure> the hierarchy of sectioning commands is
determined.  The directions implied by menus are determined with
C<set_menus_node_directions>.  The node tree is analysed with C<nodes_tree>.
Nodes directions are completed with menu directions with
C<complete_node_tree_with_menus>.  Floats get their standard numbering with
C<number_floats> and internal references are matched up with nodes, floats or
anchors with C<associate_internal_references>.

The following methods depend on the output format, so are usually called
from converters.

It is also possible to associate top-level contents of the tree, which consist
in nodes and sectioning commands with tree unit elements that group together
a node and the next sectioning element.  With C<split_by_node> nodes are considered
to be the main sectioning elements, while with C<split_by_section> the
sectioning command elements are the main elements.  The first mode is typical
of Info format, while the second corresponds to a traditional book.
The elements may be further split in I<pages>, which are not pages as
in book pages, but more like web pages, and hold series of tree unit elements.

The elements may have directions to other elements prepared
by C<elements_directions>.  C<elements_file_directions> should also
set direction related to files, provided files are associated with
elements by the user.

C<merge_indices> may be used to merge indices, which may be sorted
with C<sort_indices>.


=head1 METHODS

No method is exported in the default case.

Most methods takes a L<Texinfo::Report> C<$registrar> as argument for error
reporting.  Most also require Texinfo customization variables information,
which means an object implementing the C<get_conf> method, in practice the main
program configuration or a converter (L<Texinfo::Convert::Converter/Getting and
setting customization variables>).  Other common input arguments such as parser
information, labels or refs are obtained from a parser, see L<Texinfo::Parser>.

=over

=item associate_internal_references($registrar, $customization_information, $parser_information, $labels, $refs)
X<C<associate_internal_references>>

Verify that internal references (C<@ref> and similar without
fourth of fifth argument and menu entries) have an associated node, anchor or
float.  Set the C<normalized> key in the C<extra> hash C<menu_entry_node> hash
for menu entries and in the C<extra> hash C<node_argument> hash for internal
references C<@ref> and similar @-commands.  Set the C<label> key in the
C<extra> hash of the reference tree element to the associated labeled tree
element.  Register errors in I<$registrar>.

=item check_nodes_are_referenced($registrar, $customization_information, $nodes_list, $top_node, $labels, $refs)
X<C<check_nodes_are_referenced>>

Check that all the nodes are referenced (in menu, @*ref or node direction).
Register errors in I<$registrar>.

Should be called after C<complete_node_tree_with_menus> in order to
have the autogenerated menus available.

=item complete_node_tree_with_menus($registrar, $customization_information, $nodes_list, $top_node)
X<C<complete_node_tree_with_menus>>

Complete nodes directions with menu directions.  Check consistency
of menus, sectionning and nodes direction structures.
Register errors in I<$registrar>.

=item elements_directions($customization_information, $labels, $tree_units)
X<C<elements_directions>>

Directions are set up for the tree unit elements in the array reference
I<$tree_units> given in argument.  The corresponding hash is in
C<< {'structure'}->{'directions'} >>
and keys correspond to directions while values are elements.

The following directions are set up:

=over

=item This

The element itself.

=item Forward

Element next.

=item Back

Previous element.

=item NodeForward

Following node element in reading order.  It is the next node, or the
first in menu or the next of the up node.

=item NodeBack

Preceding node element.

=item NodeUp

=item NodeNext

=item NodePrev

The up, next and previous node elements.

=item Up

=item Next

=item Prev

The up, next and previous section elements.

=item FastBack

For top level elements, the previous top level element.  For other elements
the up top level element.  For example, for a chapter element it is the
previous chapter, for a subsection element it is the chapter element
that contains the subsection.

=item FastForward

The next top level section element.

=back

=item elements_file_directions($tree_units)
X<C<elements_file_directions>>

In the directions reference described above for C<elements_directions>, sets
the I<PrevFile> and I<NextFile> directions to the elements in previous and
following files.

It also sets I<FirstInFile*> directions for all the elements by using
the directions of the first element in file.  So, for example,
I<FirstInFileNodeNext> is the next node of the first element in
the file of each element.

The API for association of pages/elements to files is not defined yet.

=item @nodes_list = get_node_node_childs_from_sectioning($node)
X<C<get_node_node_childs_from_sectioning>>

I<$node> is a node tree element.  Find the node I<$node> children based
on the sectioning structure.  For the node associated with C<@top>
sectioning command, the sections associated with parts are considered.

=item $entry_key = index_entry_sort_string($main_entry, $entry_tree_element, $sortas, $options)
X<C<index_entry_sort_string>>

Return a string suitable as a sort string, for index entries.
The index entry processed is I<$entry_tree_element>, and can be a
C<@subentry>.  I<$main_entry> is the main index entry tree element
that can be used to gather information.  I<$sortas> can be given to
override the sort string (typically obtained from C<@sortas>).   The
I<$options> are options used for Texinfo to text conversion for
the generation of the sort string, typically obtained from
L<setup_index_entry_keys_formatting|/$option = setup_index_entry_keys_formatting($customization_information)>.

=item $merged_entries = merge_indices($index_names)
X<C<merge_indices>>

Using information returned by L<Texinfo::Parser/indices_information>,
a structure holding all the index entries by index name is returned,
with all the entries of merged indices merged with those of the indice
merged into.

The I<$merged_entries> returned is a hash reference whose
keys are the index names and values arrays of index entry structures
described in details in L<Texinfo::Parser/index_entries>.

=item $new_block = new_block_command($content, $parent, $command_name)
X<C<new_block_command>>

Returns the texinfo tree corresponding to a block command named
I<$command_name> with contents I<$content> and parent in tree I<$parent>.

=item $new_menu = new_complete_node_menu($node, $use_sections)
X<C<new_complete_node_menu>>

Returns a texinfo tree menu for node I<$node>, pointing to the children
of the node obtained with the sectioning structure.  If I<$use_sections>
is set, use section names for the menu entry names.

=item $entry = new_node_menu_entry($node, $use_sections)
X<C<new_node_menu_entry>>

Returns the texinfo tree corresponding to a single menu entry pointing to
I<$node>.  If I<$use_sections> is set, use the section name for the menu
entry name.  Returns C<undef> if the node argument is missing.

=item $top_node = nodes_tree($registrar, $customization_information, $parser_information, $nodes_list, $labels)
X<C<nodes_tree>>

Goes through nodes and set directions.  Returns the top
node.  Register errors in I<$registrar>.

This functions sets, in the C<structure> node element hash:

=over

=item node_up

=item node_prev

=item node_next

Up, next and previous directions for the node.

=back

=item number_floats($float_information)
X<C<number_floats>>

Number the floats as described in the Texinfo manual.  Sets
the I<number> key in the C<structure> hash of the float
tree elements.

=item $command_name = section_level_adjusted_command_name($element)
X<C<section_level_adjusted_command_name>>

Return the sectioning command name corresponding to the sectioning
element I<$element>, adjusted in order to take into account raised
and lowered sections, when needed.

=item $sections_root, $sections_list = sectioning_structure($registrar, $customization_information, $tree)
X<C<sectioning_structure>>

This function goes through the tree and gather information on
the document structure for sectioning commands.  It returns I<$sections_root>
the root of the sectioning commands tree and a reference on the sections
elements list.  Errors are registered in I<$registrar>.

It sets section elements C<structure> hash values:

=over

=item section_level

The level in the sectioning tree hierarchy.  0 is for C<@top> or
C<@part>, 1 for C<@chapter>, C<@appendix>...  This level is corrected
by C<@raisesections> and C<@lowersections>.

=item section_number

The sectioning element number.

=item section_childs

An array holding sectioning elements children of the element.

=item section_up

=item section_prev

=item section_next

The up, previous and next sectioning elements.

=item toplevel_next

=item toplevel_prev

=item toplevel_up

The next and previous and up sectioning elements of toplevel sectioning
elements (like C<@top>, C<@chapter>, C<@appendix>), not taking into
account C<@part> elements.

=back

=item set_menus_node_directions($registrar, $customization_information, $parser_information, $global_commands, $nodes_list, $labels);
X<C<set_menus_node_directions>>

Goes through menu and set directions.  Register errors in I<$registrar>.

This functions sets, in the C<structure> node element hash reference:

=over

=item menu_child

The first child in the menu of the node.

=item menu_up

=item menu_next

=item menu_prev

Up, next and previous directions as set in menus.

=back

=item $option = setup_index_entry_keys_formatting($customization_information)
X<C<setup_index_entry_keys_formatting>>

Return options for conversion of Texinfo to text relevant for index keys sorting.

=item ($index_entries_sorted, $index_entries_sort_strings) = sort_indices($registrar, $customization_information, $merged_index_entries, $sort_by_letter)
X<C<sort_indices>>

If I<$sort_by_letter> is set, sort by letter, otherwise sort all
entries together.  In both cases, a hash reference with index names
as keys I<$index_entries_sorted> is returned.

When sorting by letter, an array reference of letter hash references is
associated with each index name.  Each letter hash reference has two
keys, a I<letter> key with the letter, and an I<entries> key with an array
reference of sorted index entries beginning with the letter.

When simply sorting, the array of the sorted index entries is associated
with the index name.

I<$index_entries_sort_strings> is a hash reference associating the index
entries with the strings that were used to sort them.

Register errors in I<$registrar>.

=item $tree_units = split_by_node($tree)
X<C<split_by_node>>

Returns a reference array of tree units where a node is associated to
the following sectioning commands.  Sectioning commands without nodes
are also with the previous node, while nodes without sectioning commands
are alone in their tree units.

Tree units are regular tree elements with type I<unit>, the
associated nodes and sectioning tree elements are in the array associated
with the C<contents> key.  The associated elements have a I<associated_unit>
key set in the C<structure> hash that points to the associated tree unit.

Tree units have directions in the C<structure>
hash reference, namely I<unit_next> and I<unit_prev> pointing to the
previous and the next tree unit.

In the C<extra> hash reference, tree units have:

=over

=item unit_command

The node command associated with the element.

=back

=item $tree_units = split_by_section($tree)
X<C<split_by_section>>

Similarly with C<split_by_node>, returns an array of tree units.  This time,
lone nodes are associated with the previous sections and lone sections
makes up a tree unit.

The C<structure> and C<extra> hash keys set are the same, except that I<unit_command> is
the sectioning command associated with the element.

=item $pages = split_pages($tree_units, $split)
X<C<split_pages>>

The tree units from the array reference argument have an extra
I<first_in_page> value set in the C<structure> hash reference to
the first tree unit in the group, and based on the value of I<$split>.
The possible values for I<$split> are

=over

=item chapter

The tree units are split at chapter or other toplevel sectioning tree units.

=item node

Each element has its own page.

=item section

The tree units are split at sectioning commands below chapter.

=item value evaluating to false

No splitting, only one page is returned, holding all the tree units.

=back

=item warn_non_empty_parts($registrar, $customization_information, $global_commands)
X<C<warn_non_empty_parts>>

Register a warning in I<$registrar> for each C<@part> that is not empty
in I<$global_commands> information (typically obtained by calling
C<global_commands_information()> on a parser).

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
