Fixed a bug? Implemented a new feature? Want to have it included in QuaZIP?
Here's what you need to do.

0. If you don't have a GitHub account, create one. It's ridiculously easy.

1. First, open an [issue](https://github.com/stachenov/quazip/issues).
Even if you have already fixed it. It helps to track things because
instead of a commit saying “fixed this and that” you have a reference
to a full issue description.

2. Next, send a pull request. This is sort of counter-intuitive, so if
you never did it, read on.

Pull requests (PRs) are GitHub's way of sending patches to people. Except that
instead of sending a patch, you send a full-fledged version control branch
that is much easier to integrate than a regular patch.

Here are the steps you need to follow if you're sending your first PR to QuaZIP.

0. If you haven't installed Git yet, do so. In Windows, choose the recommended
setting for line endings.
1. Go to https://github.com/stachenov/quazip.
2. Click “Fork” in the top-right corner.
3. You should now have a full copy of QuaZIP in your own GitHub account.
This may sound like a huge overkill to just send a small patch, but it's
actually very convenient once you get used to it.
4. Clone QuaZIP to your computer. Use your own account's repo URL as a source,
NOT https://github.com/stachenov/quazip!
```
git clone https://github.com/your-user-name-goes-here/quazip
```
This command will create a new `quazip` directory and clone into it.
Now you have two QuaZIP repos. One in your GitHub account, accessible to
you only via GitHub itself, and another one in the directory that has
just been created.

5. Go into this new directory and run
```
git remote add upstream https://github.com/stachenov/quazip
```
Check that upstream was added correctly:
```
git remote -v
```
It should show something like
```
origin  https://github.com/your-user-name/quazip (fetch)
origin  https://github.com/your-user-name/quazip (push)
upstream        https://github.com/stachenov/quazip (fetch)
upstream        https://github.com/stachenov/quazip (push)
```

Now your local repo knows how to exchange commits with your GitHub
account and main QuaZIP repo. You're all set for submitting your first PR.

If you ever wish to submit another PR, no need to do all this again.
Except that if you delete your local repo, you'll have to redo steps 4 and 5.

Submitting a PR is easy, but can be confusing first time if you're
not familiar with Git.

1. Create a new branch in your local repo. This is very important!
Most Git repos, including QuaZIP, have the main branch called “master”.
By creating a new branch you tell Git where your changes begin and end.
This way GitHub can automatically create a patch containing only your
changes. To create a branch, run from your local repo:
```
git checkout -b new-branch-name
```
Name the branch whatever you want, it's totally unimportant as it will
be deleted later. If you already have some changes in your working
dir, but have not added/committed them, it's still not too late to create
a new branch, your changes will be kept.

2. Make whatever changes you want.

3. Run `git add` with all changed/added files as arguments. `git status`
will remind you what files you have changed or added. After you're done,
`git status` should show all your files as “changes to be committed”.
Note that if you change a file, add it, then change it again,
you need to re-run `git add` or you end up committing your file
as it was when you first added it (Git remembers its contents).

4. Run `git commit`. Enter a meaningful commit message. See Git docs
for examples and recommendations.

5. Run
```
git push -u origin your-branch-name
```
This weird command sends your committed changes to your GitHub account.
The `-u` option tells Git to remember that your branch goes to your
account under the same name, so the next time just `git push` will be enough.

6. Go to your account's page and open quazip there. You should see a message
informing you about the new branch and offering to create a PR. Just do it.
Don't be afraid to mess something up as your changes aren't going anywhere yet.
If there's something wrong with them, I'll just tell you what and how to fix.
