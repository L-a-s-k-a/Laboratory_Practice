# Translations.pm: translate strings.
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

package Texinfo::Translations;

use 5.00405;
use strict;

# To check if there is no erroneous autovivification
#no autovivification qw(fetch delete exists store strict);

use Encode;
use POSIX qw(setlocale LC_ALL LC_MESSAGES);
use Locale::Messages;

# note that there is a circular dependency with the parser module, as
# the parser uses complete_indices() from this modules, while this modules
# can use a parser.  This is not problematic, however, as the
# modules do not setup data such that their order of loading is not
# important, as long as they load after their dependencies.

# to be able to load a (simple) parser if none was given to gdt.
use Texinfo::Parser;

# we want a reliable way to switch locale for the document
# strings translations so we don't use the system gettext.
Locale::Messages->select_package ('gettext_pp');

# i18n

my $DEFAULT_LANGUAGE = 'en';

my $DEFAULT_ENCODING = 'utf-8';
my $DEFAULT_PERL_ENCODING = 'utf-8';

my $messages_textdomain = 'texinfo';
my $strings_textdomain = 'texinfo_document';

# libintl converts between encodings but doesn't decode them into the
# perl internal format.  This is only called if the encoding is a proper
# perl encoding.
sub _decode_i18n_string($$)
{
  my $string = shift;
  my $encoding = shift;
  return Encode::decode($encoding, $string);
}

sub _switch_messages_locale
{
  my $locale;
  our $working_locale;
  if ($working_locale) {
    $locale = POSIX::setlocale(LC_MESSAGES, $working_locale);
  }
  if (!$locale) {
    $locale = POSIX::setlocale(LC_MESSAGES, "en_US.UTF-8");
  }
  if (!$locale) {
    $locale = POSIX::setlocale(LC_MESSAGES, "en_US")
  }
  # try the output of 'locale -a' (but only once)
  our $locale_command;
  if (!$locale and !$locale_command) {
    $locale_command = "locale -a";
    my @local_command_locales = split("\n", `$locale_command`);
    if ($? == 0) {
      foreach my $try (@local_command_locales) {
        next if $try eq 'C' or $try eq 'POSIX';
        $locale = POSIX::setlocale(LC_MESSAGES, $try);
        last if $locale;
      }
    }
  }
  $working_locale = $locale;
}

# Get document translation - handle translations of in-document strings.
# Return a parsed Texinfo tree
sub gdt($$;$$$)
{
  my ($self, $message, $replaced_substrings, $type, $lang) = @_;

  # In addition to being settable from the command line,
  # the language needs to be dynamic in case there is an untranslated string
  # from another language that needs to be translated.
  $lang = $self->get_conf('documentlanguage') if ($self and !defined($lang));
  $lang = $DEFAULT_LANGUAGE if (!defined($lang));

  my $re = join '|', map { quotemeta $_ } keys %$replaced_substrings
      if (defined($replaced_substrings) and ref($replaced_substrings));

  my ($saved_LC_MESSAGES, $saved_LANGUAGE);

  # We need to set LC_MESSAGES to a valid locale other than "C" or "POSIX"
  # for translation via LANGUAGE to work.  (The locale is "C" if the
  # tests are being run.)
  #   LC_MESSAGES was reported not to exist for Perl on MS-Windows.  We
  # could use LC_ALL instead, but (a) it's not clear if this would help,
  # and (b) this could interfere with the LC_CTYPE setting in XSParagraph.

  if ($^O ne 'MSWin32') {
    $saved_LC_MESSAGES = POSIX::setlocale(LC_MESSAGES);
    _switch_messages_locale();
  }
  $saved_LANGUAGE = $ENV{'LANGUAGE'};

  Locale::Messages::textdomain($strings_textdomain);

  # FIXME do this only once when encoding is seen (or at beginning)
  # instead of here, each time that gdt is called?
  my $encoding;
  my $perl_encoding;
  if ($self) {
    if ($self->get_conf('OUTPUT_ENCODING_NAME')) {
      $encoding = $self->get_conf('OUTPUT_ENCODING_NAME');
    }
    if ($self->get_conf('OUTPUT_PERL_ENCODING')) {
      $perl_encoding = $self->get_conf('OUTPUT_PERL_ENCODING');
    }
  } else {
    $encoding = $DEFAULT_ENCODING;
    $perl_encoding = $DEFAULT_PERL_ENCODING;
  }
  Locale::Messages::bind_textdomain_codeset($strings_textdomain, $encoding)
    if ($encoding and $encoding ne 'us-ascii');
  if (!($encoding and $encoding eq 'us-ascii')) {
    if ($perl_encoding) {
      Locale::Messages::bind_textdomain_filter($strings_textdomain,
        \&_decode_i18n_string, $perl_encoding);
    }
  }

  my @langs = ($lang);
  if ($lang =~ /^([a-z]+)_([A-Z]+)/) {
    my $main_lang = $1;
    my $region_code = $2;
    push @langs, $main_lang;
  }

  my @locales;
  foreach my $language (@langs) {
    if ($encoding) {
      push @locales, "$language.$encoding";
    } else {
      push @locales, $language;
    }
    # always try us-ascii, the charset should always be a subset of
    # all charset, and should resort to @-commands if needed for non
    # ascii characters
    # REMARK this is not necessarily true for every language/encoding.
    # This can be true for latin1, and maybe some other 8 bit encodings
    # with accents available as @-commands, but not for most
    # language.  However, for those languages, it is unlikely that
    # the locale with .us-ascii can be set, so it should not hurt
    # to add this possibility.
    if (!$encoding or ($encoding and $encoding ne 'us-ascii')) {
      push @locales, "$language.us-ascii";
    }
  }

  my $locales = join(':', @locales);

  Locale::Messages::nl_putenv("LANGUAGE=$locales");

  my $translated_message = Locale::Messages::gettext($message);

  Locale::Messages::textdomain($messages_textdomain);

  if (!defined($saved_LANGUAGE)) {
    delete ($ENV{'LANGUAGE'});
  } else {
    $ENV{'LANGUAGE'} = $saved_LANGUAGE;
  }

  if ($^O ne 'MSWin32') {
    if (defined($saved_LC_MESSAGES)) {
      POSIX::setlocale(LC_MESSAGES, $saved_LC_MESSAGES);
    } else {
      POSIX::setlocale(LC_MESSAGES, '');
    }
  }

  my $translation_result = $translated_message;

  if ($type and $type eq 'translated_text') {
    if (defined($re)) {
      # next line taken from libintl perl, copyright Guido. sub __expand
      $translation_result
  =~ s/\{($re)\}/defined $replaced_substrings->{$1} ? $replaced_substrings->{$1} : "{$1}"/ge;
    }
    return $translation_result;
  }

  # we change the substituted brace-enclosed strings to internal
  # values marked by @txiinternalvalue such that their location
  # in the Texinfo tree can be marked.  They are substituted
  # after the parsing in the final tree.
  # Using a special command that is invalid unless a special
  # configuration is set means that there should be no clash
  # with @-commands used in translations.
  if (defined($re)) {
    # next line taken from libintl perl, copyright Guido. sub __expand
    $translation_result =~ s/\{($re)\}/\@txiinternalvalue\{$1\}/g;
  }

  # FIXME not sure if the added complexity of getting information from parser
  # is worth it.  The current use case, that is allow to specify the state of
  # clickstyle and kbdinputstyle is relevant (though not implemented in thet XS
  # parser, but could be) but not necessarily determining.  Converters and
  # users could easily avoid using @kbd and @click in the translated messages.
  # FIXME why not use $self->get_conf('clickstyle'), ...?  It would not be used
  # everytime, only if and where the $self object sets 'clickstyle'
  # and 'kbdinputstyle'
  # FIXME currently, converters are not associated with parser, such
  # that the second condition should always be false

  # determine existing parser, if any
  my $current_parser;
  if ($self) {
    if (ref($self) eq 'Texinfo::Parser') {
      $current_parser = $self;
    } elsif ($self->{'parser'}) {
      $current_parser = $self->{'parser'};
    }
  }

  # We never reuse a parser directly as it is cleaner to get only the
  # relevant information (if any).  It could also mess with the parser
  # state, though this has not been checked for a long time.

  # accept @txiinternalvalue as a valid Texinfo command, used to mark
  # location in tree of substituted brace enclosed strings.
  my $parser_conf = {'accept_internalvalue' => 1};
  # information only found in parser, not available through get_conf().
  # Note that it is only available for the NonXS parser.
  if ($current_parser) {
    foreach my $duplicated_conf ('clickstyle', 'kbdinputstyle') {
      $parser_conf->{$duplicated_conf} = $current_parser->{$duplicated_conf}
        if (defined($current_parser->{$duplicated_conf}));
    }
  }
  # general customization relevant for parser
  if ($self) {
    foreach my $conf_variable ('DEBUG') {
      if (defined($self->get_conf($conf_variable))) {
        $parser_conf->{$conf_variable} = $self->get_conf($conf_variable);
      }
    }
  }
  my $parser = Texinfo::Parser::simple_parser($parser_conf);
  if ($parser->{'DEBUG'}) {
    print STDERR "GDT $translation_result\n";
  }

  my $tree = $parser->parse_texi_line($translation_result);
  my $registrar = $parser->registered_errors();
  my ($errors, $errors_count) = $registrar->errors();
  if ($errors_count) {
    warn "translation $errors_count error(s)\n";
    warn "translated message: $translated_message\n";
    warn "Error messages: \n";
    foreach my $error_message (@$errors) {
      warn $error_message->{'error_line'};
    }
  }
  $tree = _substitute ($tree, $replaced_substrings);
  return $tree;
}

sub _substitute_element_array ($$);
sub _substitute_element_array ($$) {
  my $array = shift; my $replaced_substrings = shift;

  @{$array} = map {
    if ($_->{'cmdname'} and $_->{'cmdname'} eq 'txiinternalvalue') {
      my $name = $_->{'args'}->[0]->{'text'};
      if (ref($replaced_substrings->{$name}) eq 'HASH') {
        $replaced_substrings->{$name};
      } elsif (ref($replaced_substrings->{$name}) eq 'ARRAY') {
        @{$replaced_substrings->{$name}};
      } elsif (ref($replaced_substrings->{$name}) eq '') {
        {'text' => $replaced_substrings->{$name}};
      } else {
        (); # undefined - shouldn't happen?
      }
    } else {
      _substitute($_, $replaced_substrings);
      ( $_ );
    }
  } @{$array};
}

# Recursively substitute @txiinternalvalue elements in $TREE with
# their values given in $CONTEXT.
sub _substitute ($$);
sub _substitute ($$) {
  my $tree = shift; my $replaced_substrings = shift;

  if ($tree->{'contents'}) {
    _substitute_element_array ($tree->{'contents'}, $replaced_substrings);
  }

  if ($tree->{'args'}) {
    _substitute_element_array ($tree->{'args'}, $replaced_substrings);
  }

  return $tree;
}


sub _non_bracketed_contents($) {
  my $current = shift;

  if ($current->{'type'} and $current->{'type'} eq 'bracketed') {
    my $new = {};
    $new->{'contents'} = $current->{'contents'} if ($current->{'parent'});
    $new->{'parent'} = $current->{'parent'} if ($current->{'parent'});
    return $new;
  } else {
    return $current;
  }
}

if (0) {
  # it is needed to mark the translation as gdt is called like
  # gdt($self, '....')
  # and not like gdt('....')
  gdt('{name} on {class}', undef, undef);
  gdt('{name} of {class}', undef, undef);
}

# In a handful of cases, we delay storing the contents of the
# index entry until now to avoid needing Texinfo::Translations::gdt
# in the main code of Parser.pm.  Also set 'in_code' value on
# index entries.
sub complete_indices($)
{
  my $self = shift;

  foreach my $index_name (keys(%{$self->{'index_names'}})) {
    next if (not defined($self->{'index_names'}->{$index_name}->{'index_entries'}));
    foreach my $entry (@{$self->{'index_names'}->{$index_name}->{'index_entries'}}) {
      $entry->{'in_code'} = $self->{'index_names'}->{$index_name}->{'in_code'};
      if (not defined($entry->{'entry_content'})) {
        my ($index_entry, $index_contents_normalized);
        my $def_command = $entry->{'entry_element'}->{'extra'}->{'def_command'};

        my $def_parsed_hash = $entry->{'entry_element'}->{'extra'}->{'def_parsed_hash'};
        if ($def_parsed_hash and $def_parsed_hash->{'class'}
            and $def_command) {
          # Use the document language that was current when the command was
          # used for getting the translation.
          my $entry_language
             = $entry->{'entry_element'}->{'extra'}->{'documentlanguage'};
          if ($def_command eq 'defop'
              or $def_command eq 'deftypeop'
              or $def_command eq 'defmethod'
              or $def_command eq 'deftypemethod') {
            $index_entry = gdt($self, '{name} on {class}',
                                      {'name' => $def_parsed_hash->{'name'},
                                       'class' => $def_parsed_hash->{'class'}},
                                      undef, $entry_language);
            $index_contents_normalized
              = [_non_bracketed_contents($def_parsed_hash->{'name'}),
                { 'text' => ' on '},
                _non_bracketed_contents($def_parsed_hash->{'class'})];
          } elsif ($def_command eq 'defivar'
                   or $def_command eq 'deftypeivar'
                   or $def_command eq 'deftypecv') {
            $index_entry = gdt($self, '{name} of {class}',
                                      {'name' => $def_parsed_hash->{'name'},
                                       'class' => $def_parsed_hash->{'class'}},
                                      undef, $entry_language);
            $index_contents_normalized
              = [_non_bracketed_contents($def_parsed_hash->{'name'}),
                 { 'text' => ' of '},
                 _non_bracketed_contents($def_parsed_hash->{'class'})];
          }
        }
        # 'root_line' is the container returned by gdt.
        if ($index_entry->{'type'} and $index_entry->{'type'} eq 'root_line') {
          for my $child (@{$index_entry->{'contents'}}) {
            delete $child->{'parent'};
          }
        }
        if ($index_entry->{'contents'}) {
          $entry->{'entry_content'} = [@{$index_entry->{'contents'}}];
          $entry->{'content_normalized'} = $index_contents_normalized;
        }
      }
    }
  }
}



1;

__END__

=head1 NAME

Texinfo::Translations - Translations of output documents strings for Texinfo modules

=head1 SYNOPSIS

  @ISA = qw(Texinfo::Translations);

  my $tree_translated = $converter->gdt('See {reference} in @cite{{book}}',
                       {'reference' => $tree_reference,
                        'book'  => {'text' => $book_name}});


=head1 NOTES

The Texinfo Perl module main purpose is to be used in C<texi2any> to convert
Texinfo to other formats.  There is no promise of API stability.

=head1 DESCRIPTION

The C<Texinfo::Translations> module helps with translations
in output documents.

Translation of error messages uses another interface, which
is the classical gettext based perl interface.  It is not
described as it is described in details elsewhere, some
elements are in L<Texinfo::Common C<__> and C<__p>|Texinfo::Common/$translated_string = __($msgid)>.

=head1 METHODS

No method is exported.

The C<gdt> method is used to translate strings to be output in
converted documents, and returns, in general, a texinfo tree.

=over

=item $tree = $object->gdt($string, $replaced_substrings, $mode, $lang)
X<C<gdt>>

The I<$string> is a string to be translated.  In the default case,
the function returns a Texinfo tree, as the string is interpreted
as Texinfo code after translation.  I<$replaced_substrings> is an
optional hash reference specifying some substitution to be done
after the translation.  The key of the I<$replaced_substrings> hash
reference identifies what is to be substituted, and the value is
some string, texinfo tree or array content that is substituted in
the resulting texinfo tree.  In the string to be translated word
in brace matching keys of I<$replaced_substrings> are replaced.
The I<$object> is typically a converter, but can be any object that implements
C<get_conf>, or undefined (C<undef>).  If not undefined, the information in the
I<$object> is used to determine the encoding, the documentlanguage and get some
customization information. I<$lang> is optional. If set, it overrides the
documentlanguage.

=begin comment

If the I<$object> is a parser or is associated to a parser some
information may be used, but it is different for the NonXS and
XS parser, and may also not be such a good idea, therefore no
documentation.

=end comment

For example, in the following call, the string
C<See {reference} in @cite{{book}}> is translated, then
parsed as a Texinfo string, with I<{reference}> substituted by
I<$tree_reference> in the resulting tree, and I<{book}>
replaced by the associated texinfo tree text element:

  $tree = $converter->gdt('See {reference} in @cite{{book}}',
                       {'reference' => $tree_reference,
                        'book'  => {'text' => $book_name}});

C<gdt> uses a gettext-like infrastructure to retrieve the
translated strings, using the I<texinfo_document> domain.

I<$mode> is an optional string which may modify how the function
behaves.  The possible values are:

=over

=item translated_text

In that case the string is not considered to be Texinfo, a plain string
that is returned after translation and substitution.  The substitutions
may only be strings in that case.

=back

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
