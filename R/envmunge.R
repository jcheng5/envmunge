fix_yosemite_bug <- function() {
  env_names <- sub("=.*", "", ls_env())
  dupes <- duplicated(env_names)
  unset(dupes)
}