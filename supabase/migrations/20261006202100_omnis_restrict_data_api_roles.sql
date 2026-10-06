-- Applied to the dedicated Omnis Supabase project mqdrcgymylbqozarykip.
-- Restrict Data API grants to authenticated game users only.

revoke all privileges on table public.game_profiles from anon;
revoke all privileges on table public.player_progression from anon;
revoke all privileges on table public.player_saves from anon;
revoke all privileges on table public.game_events from anon;

revoke all privileges on table public.game_profiles from authenticated;
revoke all privileges on table public.player_progression from authenticated;
revoke all privileges on table public.player_saves from authenticated;
revoke all privileges on table public.game_events from authenticated;

grant select, insert, update on table public.game_profiles to authenticated;
grant select, insert, update on table public.player_progression to authenticated;
grant select, insert, update, delete on table public.player_saves to authenticated;
grant select, insert on table public.game_events to authenticated;
