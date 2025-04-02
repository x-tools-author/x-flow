{%- comment -%}
  Include as: {%- include_cached favicon.html -%}
  Depends on: site.static_files.
  Results in: HTML for a link to an existing `favicon.ico` file.
  Overwrites:
    file.

  The endoflife.date site has 226 pages and 3410 static files. @marcwrobel pointed
  out that the time taken by evaluating the code in this file on every page when
  building that site was significant, and suggested making it optional. As it is
  page-independent, it can easily be cached. Doing that reduced the time taken by
  rendering `_includes/head.html` from 15.294s to 10.760s, thereby reducing the
  total build time from 26.074s to 21.656s -- a saving of about 17%.
{%- endcomment -%}
