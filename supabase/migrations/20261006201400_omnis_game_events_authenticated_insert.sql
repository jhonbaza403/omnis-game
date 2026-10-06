-- Applied to the dedicated Omnis Supabase project mqdrcgymylbqozarykip.
-- Authenticated users may write only their own gameplay events.

create policy "authenticated users can insert own game events"
on public.game_events
for insert
to authenticated
with check ((select auth.uid()) = user_id);
