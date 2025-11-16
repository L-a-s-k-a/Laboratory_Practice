# API documentation code examples implemented, to check syntax
# and also expected effects.  Only for code that is not already elsewhere,
# nor is too complex to set up.  Some of those customization results
# are silly, just for the sake of checking.

use strict;

# To check if there is no erroneous autovivification
#no autovivification qw(fetch delete exists store strict);

my $default_footnotestyle = texinfo_get_conf('footnotestyle');
my $main_program_footnotestyle;
if (not defined($default_footnotestyle)) {
  $main_program_footnotestyle = 'is undef';
} elsif ($default_footnotestyle eq 'separate') {
  $main_program_footnotestyle = 'is separate';
} else {
  $main_program_footnotestyle = 'not separate '.$default_footnotestyle;
}

texinfo_register_no_arg_command_formatting('-', undef, '&shy;');

texinfo_register_no_arg_command_formatting('error', undef, undef, undef,
                                           'error--&gt;');

texinfo_register_style_command_formatting('sansserif', 'code', 0, 'normal');
texinfo_register_style_command_formatting('sansserif', 'code', 0, 'preformatted');
texinfo_register_style_command_formatting('sansserif', undef, 1, 'string');

texinfo_register_accent_command_formatting('dotless', 'nodot', 'ij');

my $shown_styles;
my $footnotestyle;
sub my_function_set_some_css {
  my $converter = shift;
  my @all_included_rules = $converter->css_get_info('rules');
  my $all_default_selector_styles = $converter->css_get_info('styles');
  my $titlefont_style = $all_default_selector_styles->{'h1.titlefont'};
  $titlefont_style = 'undefined' if (!defined($titlefont_style));
  $shown_styles = $titlefont_style.' '.
              $all_default_selector_styles->{'h1.shorttitlepage'};
  $converter->css_add_info('styles', 'h1.titlefont', 'text-align:center');

  my $footnotestyle_before_setting = $converter->get_conf('footnotestyle');
  $footnotestyle_before_setting = 'UNDEF'
     if (not defined($footnotestyle_before_setting));
  $converter->set_conf('footnotestyle', 'separate');
  $footnotestyle = $main_program_footnotestyle
                    .'|'.$footnotestyle_before_setting
                    .'|'.$converter->get_conf('footnotestyle');
  # there should be nothing in @all_included_rules for two reasons,
  # first because it requires 'CSS_FILES' to be set to parseable
  # CSS files, and CSS files parsing is done after the setup handler
  # is called.
  #print STDERR "all_included_rules: ".join('|', @all_included_rules)."\n";
  return 0;
}

texinfo_register_handler('setup', \&my_function_set_some_css);

sub my_email_formatting_function {
  my $converter = shift;
  my $cmdname = shift;
  my $command = shift;
  my $args = shift;

  my $mail_arg = shift @$args;
  my $text_arg = shift @$args;
  my $mail = '';
  my $mail_string;
  if (defined($mail_arg)) {
    $mail = $mail_arg->{'url'};
    $mail_string = $mail_arg->{'monospacestring'};
  }
  my $text = '';
  if (defined($text_arg)) {
    $text = $text_arg->{'normal'};
  }

  if ($converter->in_string()) {
    return "$mail_string ($text) $shown_styles, $footnotestyle";
  } else {
    return $converter->html_attribute_class('a', [$cmdname])
     ." href=\"mailto:$mail_string\">$text</a> [$shown_styles, $footnotestyle]";
  }
}

texinfo_register_command_formatting('email', \&my_email_formatting_function);

sub my_convert_paragraph_type($$$$)
{
  my $converter = shift;
  my $type = shift;
  my $element = shift;
  my $content = shift;

  return $content if ($converter->in_string());

  my @contents = @{$element->{'contents'}};
  push @contents, {'text' => ' <code>HTML</code> text ',
                   'type' => '_converted'};
  my $result = $converter->convert_tree({'type' => '_code',
                                   'contents' => \@contents });
  return "<p>".$result."</p>";
}

texinfo_register_type_formatting('paragraph', \&my_convert_paragraph_type);

sub my_node_file_name($$$) {
  my ($converter, $element, $filename) = @_;
  # ....
  my $node_file_name = 'prepended_to_filenames-'.$filename;
  return $node_file_name;
}

texinfo_register_file_id_setting_function('node_file_name',
                                          \&my_node_file_name);


sub my_label_target_name($$$) {
  my ($converter, $label_info, $default_target) = @_;
  if (defined($label_info->{'normalized'})) {
    my $element = $converter->label_command($label_info->{'normalized'});
    return 'prepended_to_labels-'.$element->{'extra'}->{'normalized'};
  }
  return $default_target;
}

texinfo_register_file_id_setting_function('label_target_name',
                                          \&my_label_target_name);

1;
